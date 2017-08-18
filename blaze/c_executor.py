import json

import cffi
from cffi import FFI
from sys import exit, platform
import numpy as np
import os

from blaze.utils import *
from blaze.constants import *
from blaze.rdd_result import NumpyResult
from numba.types import Tuple
from blaze.benchmarks.timer import Timer

BLAZE_PATH = getBlazePath()
cpp_dir = BLAZE_PATH + "cpp/"
gen_dir = BLAZE_PATH + "cpp/gen/"
gen_header_file = "c_generate_dag_plan.h"
executer_header_file = "c_execute.h"
generate_lib = "libgenerate"
execute_lib = "execute"


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


def execute(dag_dict):
    ffi = FFI()
    timer = Timer()

    dag_str = json.dumps(dag_dict, cls=RDDEncoder)

    generator_cffi = load_cffi(cpp_dir + gen_header_file, cpp_dir + generate_lib, ffi)
    dag_c = ffi.new('char[]', dag_str.encode('utf-8'))

    # generator_cffi.c_generate_dag_plan(dag_c)

    executor_cffi = load_cffi(gen_dir + executer_header_file, gen_dir + execute_lib, ffi)
    args = []
    for op in dag_dict['dag']:
        if op[OP] == 'collection_source':
            data_ptr = ffi.cast("void*", op[DATA_PTR])
            size = op[DATA_SIZE]
            args.append(data_ptr)
            args.append(size)

    timer.start()
    res = executor_cffi.c_execute(*args)
    timer.end()
    print("executing " + str(timer.diff()))

    if dag_dict[ACTION] == 'collect':
        # add a free function to the gc on the result object
        res = ffi.gc(res, executor_cffi.c_free_result)
    res = wrap_result(res, dag_dict[ACTION], dag_dict['dag'][-1]['output_type'], ffi)

    if dag_dict[ACTION] == 'collect':
        # keep the reference of ffi object to prevent gc
        res.ex = executor_cffi
    return res


def wrap_result(res, action, type_, ffi):
    if action == "collect":
        buffer_size = res.size * get_type_size(type_)
        c_buffer = ffi.buffer(res.data, buffer_size)
        np_arr = np.frombuffer(c_buffer, dtype=str(type_).replace("(", "").replace(")", ""), count=res.size)
        np_arr = np_arr.view(NumpyResult)
        np_arr.ptr = res
        return np_arr
    elif action == "reduce":
        buffer_size = 1 * get_type_size(type_)
        c_buffer = ffi.buffer(res, buffer_size)
        np_arr = np.frombuffer(c_buffer, dtype=numba_type_to_dtype(type_))
        return tuple(np_arr[0]) if isinstance(type_, Tuple) else np_arr[0]
    else:
        return res
