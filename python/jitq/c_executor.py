import json
from sys import platform

import numpy as np
from cffi import FFI
from jitq.rdd_result import NumpyResult

from jitq.utils import get_project_path, RDDEncoder, Timer

JITQ_PATH = get_project_path()
CPP_DIR = JITQ_PATH + "/backend/"
GEN_DIR = JITQ_PATH + "/backend/gen/"
GEN_HEADER_FILE = "generate_dag_plan.h"
EXECUTOR_HEADER_FILE = "execute.h"
GENERATE_LIB = "libgenerate"
EXECUTE_LIB = "execute"


def load_cffi(header, lib_path, ffi):
    cdef_from_file = None
    try:
        with open(header, 'r') as libtestcffi_header:
            cdef_from_file = libtestcffi_header.read()
    except FileNotFoundError:
        print('Unable to find "%s"' % header)
        exit(2)
    except IOError:
        print('Unable to open "%s"' % header)
        exit(3)
    finally:
        if cdef_from_file == '':
            print('File "%s" is empty' % header)
            exit(1)

    ffi.cdef(cdef_from_file)
    if platform.startswith('win'):
        lib_extension = '.dll'
    else:
        lib_extension = '.so'
    return ffi.dlopen(lib_path + lib_extension)


def wrap_result(res, type_, ffi):
    output_type_size = ffi.sizeof(ffi.typeof(res.data[0]))
    c_buffer = ffi.buffer(res.data, res.size * output_type_size)

    # TODO(sabir, 12.06.2018):
    # if the row type is just an array and every row is of the same size
    # we should reshape the array to 2d array ("collapse the two arrays")
    # the dtype is then the dtype of the inner array
    # otherwise the dtype should be object and every object is then a numpy
    # array

    # here we should cast it to the right dtype
    np_arr = np.frombuffer(c_buffer, dtype=type_, count=res.size)
    np_arr = np_arr.view(NumpyResult)
    np_arr.ptr = res
    return np_arr


class Executor:
    """
    Singleton class to execute query
    Keeps track of generated code to avoid recompilation
    """

    class __Inner:
        def __init__(self):
            self.lib_counter = 0

        def get_executor(self, context, dag_str, conf_str):
            cache_key = dag_str + conf_str
            executor = context.executor_cache.get(cache_key, None)
            if not executor:
                ffi = FFI()
                generator = load_cffi(CPP_DIR + "src/" + GEN_HEADER_FILE,
                                      CPP_DIR + "build/" + GENERATE_LIB,
                                      ffi)
                dag_c = ffi.new('char[]', dag_str.encode('utf-8'))
                conf_c = ffi.new('char[]', conf_str.encode('utf-8'))

                timer = Timer()
                timer.start()
                generator.generate_dag_plan(
                    conf_c, dag_c, self.lib_counter)
                timer.end()
                print("time: calling make " + str(timer.diff()))
                executor = load_cffi(
                    GEN_DIR + EXECUTOR_HEADER_FILE,
                    GEN_DIR + EXECUTE_LIB + str(self.lib_counter), ffi)
                self.lib_counter += 1
                context.executor_cache[cache_key] = executor
            return executor

        def execute(self, context, dag_dict, inputs, output_type):
            ffi = FFI()

            # here we should pass the actual C type for our input tuple
            args = []
            for inpt in inputs:
                data_ptr = ffi.cast("void*", inpt[0])
                args.append(data_ptr)
                args.append(inpt[1])

            dag_str = json.dumps(dag_dict, cls=RDDEncoder)
            conf_str = json.dumps(context.conf)

            executor = self.get_executor(context, dag_str, conf_str)

            timer = Timer()
            timer.start()
            res = executor.execute(*args)
            # Add a free function to the result object that allows the C++
            # layer to clean up
            res = ffi.gc(res, executor.free_result)

            timer.end()
            print("execute " + str(timer.diff()))
            res = wrap_result(res, output_type, ffi)

            # Keep the reference of FFI library, which contains the free
            # function that is called by GC when the object goes out of scope
            res.ex = executor

            return res

    instance = None

    def __init__(self):
        if not Executor.instance:
            Executor.instance = Executor.__Inner()

    def __getattr__(self, name):
        return getattr(self.instance, name)
