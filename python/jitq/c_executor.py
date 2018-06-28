import json
from functools import reduce
from sys import platform

import numpy as np
from cffi import FFI
from numba import types

from jitq.rdd_result import NumpyResult

from jitq.utils import get_project_path, RDDEncoder, Timer, \
    numba_type_to_dtype, get_type_size

JITQ_PATH = get_project_path()
CPP_DIR = JITQ_PATH + "/backend/"
GEN_DIR = JITQ_PATH + "/backend/gen/"
GEN_HEADER_FILE = "compiler/generate_executable.h"
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


# pylint: disable=inconsistent-return-statements
def wrap_result(res, type_, ffi):

    values = json.loads(ffi.string(res).decode('utf-8'))
    assert len(values) == 1
    value = values[0]

    if isinstance(type_, types.Array):
        data_ptr = ffi.cast("void *", value['data'])
        shape = value['shape']
        assert type_.ndim == len(shape)
        dtype_size = get_type_size(type_.dtype)
        total_count = reduce(lambda t1, t2: t1 * t2, shape)
        c_buffer = ffi.buffer(data_ptr, total_count * dtype_size)
        np_arr = np.frombuffer(c_buffer,
                               dtype=numba_type_to_dtype(type_.dtype),
                               count=total_count)
        np_arr = np_arr.view(NumpyResult)
        np_arr.ptr = res
        return np_arr
    assert False


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
                generator.GenerateExecutable(
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

            args_c = ffi.new('char[]', json.dumps(inputs).encode('utf-8'))
            dag_str = json.dumps(dag_dict, cls=RDDEncoder)
            conf_str = json.dumps(context.conf)

            executor = self.get_executor(context, dag_str, conf_str)

            timer = Timer()
            timer.start()
            res = executor.execute(args_c)
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
