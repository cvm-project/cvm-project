import json

from cffi import FFI
from sys import exit, platform
import numpy as np

from blaze.utils import *

ffi = FFI()
cpp_dir = "cpp/"
gen_dir = "cpp/gen/"
gen_header_file = "c_generate_dag_plan.h"
executer_header_file = "c_execute.h"
generate_lib = "libgenerate"
execute_lib = "execute"


def load_cffi(header, lib_path):
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
    dagStr = json.dumps(dag_dict, cls=RDDEncoder)
    # call the libgenerate.so/generate_dag_plan
    # the execution plan is then in executor.so
    generatorCFFI = load_cffi(cpp_dir + gen_header_file, cpp_dir + generate_lib)
    dag_c = ffi.new('char[]', dagStr.encode('utf-8'))
    # generatorCFFI.c_generate_dag_plan(dag_c)
    # call executor.so, the return should be the address with the result
    executerCFFI = load_cffi(gen_dir + executer_header_file, gen_dir + execute_lib)
    res = executerCFFI.c_execute()
    res = ffi.gc(res, executerCFFI.free_result)
    # return wrap_result(res, dag_dict[ACTION], dag_dict['dag'][-1]['output_type'])
    return

def wrap_result(res, action, type_):
    if action == "collect":
        buffer_size = res.size * get_type_size(type_)
        c_buffer = ffi.buffer(res.data, buffer_size)
        np_arr = np.frombuffer(c_buffer, dtype=numba_type_to_dtype(type_))
        return np_arr
    elif action == "count":
        return res
