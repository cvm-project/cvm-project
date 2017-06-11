import sys
import os
import cffi_ext.operators as ops
from cffi_ext.operators import ffi, lib
from numba import jit
from numba import cffi_support

cffi_support.register_module(ops)

@jit()
def func():
    left = ffi.new('int[]', [3,4,5])
    right = ffi.new('int[]', [4,5,7])
    size = ffi.new('int*', 1)
    res = lib.intersect(left, right, 3, 3, size)
    for i in range(0, size[0]):
        print(res[i])

func()

llvm = func.inspect_llvm()
with open('passing_list.ll', 'w') as file:
    for k, v in llvm.items():
        file.write(v)