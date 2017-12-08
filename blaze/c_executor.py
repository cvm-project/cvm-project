import json

import gc
from cffi import FFI
from sys import exit, platform
import numpy as np

from blaze.utils import *
from blaze.constants import *
from blaze.rdd_result import NumpyResult
from numba.types import Tuple
from blaze.benchmarks.timer import Timer

BLAZE_PATH = getBlazePath()
cpp_dir = BLAZE_PATH + "/cpp/"
gen_dir = BLAZE_PATH + "/cpp/gen/"
gen_header_file = "generate_dag_plan.h"
executer_header_file = "c_execute.h"
generate_lib = "libgenerate"
execute_lib = "execute"

# static counter used to differentiate share lib versions
lib_counter = 0
dag_cache = {}


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


# 18 without compilation
def execute(dag_dict, hash_, inputs):
    global lib_counter

    ffi = FFI()
    args = []
    for inpt in inputs:
        data_ptr = ffi.cast("void*", inpt[0])
        size = inpt[1]
        args.append(data_ptr)
        args.append(size)

    executor_cffi = dag_cache.get(hash_, None)
    if not executor_cffi:
        dag_str = json.dumps(dag_dict, cls=RDDEncoder)
        generator_cffi = load_cffi(cpp_dir + gen_header_file, cpp_dir + generate_lib, ffi)
        dag_c = ffi.new('char[]', dag_str.encode('utf-8'))

        timer = Timer()
        timer.start()
        generator_cffi.generate_dag_plan(dag_c, lib_counter)
        timer.end()
        print("calling make " + str(timer.diff()))
        executor_cffi = load_cffi(gen_dir + executer_header_file, gen_dir + execute_lib + str(lib_counter), ffi)
        lib_counter += 1
        dag_cache[hash_] = executor_cffi

    timer = Timer()
    timer.start()
    res = executor_cffi.c_execute(*args)

    timer.end()
    print("execute " + str(timer.diff()))
    if dag_dict[ACTION] == 'collect':
        # add a free function to the gc on the result object
        res = ffi.gc(res, executor_cffi.c_free_result)
        pass
    res = wrap_result(res, dag_dict[ACTION], dag_dict['dag'][-1]['output_type'], ffi)

    if dag_dict[ACTION] == 'collect':
        # keep the reference of ffi object to prevent gc
        res.ex = executor_cffi
    # timer.end()
    # print("this " + str(timer.diff()))

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
        try:
            return tuple(np_arr[0]) if isinstance(type_, Tuple) else np_arr[0]
        except TypeError:
            return np_arr
    else:
        return res
