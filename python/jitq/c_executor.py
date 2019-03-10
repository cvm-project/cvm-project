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
GEN_HEADER_FILE = "generate_executable.h"
EXECUTOR_HEADER_FILE = "execute.h"
GENERATE_LIB = "libgenerate"
EXECUTE_LIB = "libexecute"


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

    if values[0]['type'] == 'none':
        return None

    assert values[0]['type'] == 'tuple'
    result = values[0]['fields']

    if isinstance(type_, types.Array):
        assert len(result) == 1
        outer_shape = result[0]['outer_shape']
        shape = result[0]['shape']
        assert type_.ndim == len(outer_shape)
        for (size1, size2) in zip(outer_shape, shape):
            assert size1 == size2
        for offset in result[0]['offsets']:
            assert offset == 0
        dtype_size = get_type_size(type_.dtype)
        total_count = reduce(lambda t1, t2: t1 * t2, outer_shape)
        c_buffer = ffi.buffer(ffi.cast("void *", result[0]['data']),
                              total_count * dtype_size)
        np_arr = np.frombuffer(c_buffer,
                               dtype=numba_type_to_dtype(type_.dtype),
                               count=total_count)
        np_arr = np_arr.view(NumpyResult)
        np_arr.ptr = res

        return np_arr

    if isinstance(type_, types.Tuple):
        return tuple((field['value'] for field in result))

    if isinstance(type_, (types.Integer, types.Float, types.Boolean)):
        assert len(result) == 1
        return result[0]['value']

    assert False


class ExecutorManager:
    """
    Singleton class to execute query
    Keeps track of generated code to avoid recompilation
    """

    class __Inner:
        def __init__(self):
            self.lib_counter = 0
            self.libgenerate = load_cffi(CPP_DIR + "src/" + GEN_HEADER_FILE,
                                         CPP_DIR + "build/" + GENERATE_LIB,
                                         FFI())

        def get_executor(self, context, dag_str, conf_str):
            cache_key = dag_str + conf_str
            executor = context.executor_cache.get(cache_key, None)
            if not executor:
                ffi = FFI()
                dag_c = ffi.new('char[]', dag_str.encode('utf-8'))
                conf_c = ffi.new('char[]', conf_str.encode('utf-8'))

                timer = Timer()
                timer.start()
                self.libgenerate.GenerateExecutable(
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
            res = ffi.gc(res, self.libgenerate.FreeResult)

            timer.end()
            print("execute " + str(timer.diff()))
            res = wrap_result(res, output_type, ffi)

            return res

    instance = None

    def __init__(self):
        if not ExecutorManager.instance:
            ExecutorManager.instance = ExecutorManager.__Inner()

    def __getattr__(self, name):
        return getattr(self.instance, name)
