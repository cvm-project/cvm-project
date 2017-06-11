import sys
import os
import cffi_ext.operators as ops
from numba import njit, jit
from numba import cffi_support

cffi_support.register_module(ops)
add = ops.lib.add  # THIS is really important


@jit(nopython=True)
def func():
    a = 1120
    b = 27
    res = add(a, b)  # Do not call ops.lib.add here
    print(res)


func()
print(func.inspect_types())

llvm = func.inspect_llvm()
with open('add_numba.ll', 'w') as file:
    for k, v in llvm.items():
        file.write(v)
