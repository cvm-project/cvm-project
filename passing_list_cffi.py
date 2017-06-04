import sys
import os
from cffi_ext.operators import ffi, lib

left = ffi.new('int[]', [i for i in range(0, 10)])
right = ffi.new('int[]', [4,5,6])
size = ffi.new('int*', 1)
res = lib.intersect(left, right, 10, 3, size)
for i in range(0, size[0]):
    print(res[i])