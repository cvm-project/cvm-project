import sys
import os
from time import sleep

import math

from blaze.blaze_context import *
from numba import cfunc

# sys.path.insert(0, "/home/sabir/projects/")
from numba import njit
import llvmlite.binding as llvm
import numba as nb


def my_gen():
    yield 42


# #
# data = generator(my_gen)
data = range_(0, 10, 1)

# os.environ["NUMBA_DUMP_IR"] = '1'
# os.environ["NUMBA_DUMP_ASSEMBLY"] = '1'
# os.environ["NUMBA_DUMP_OPTIMIZED"] = '1'


# os.environ["NUMBA_DUMP_LLVM"] = '1'

w = data.map(lambda w: (w, w + 1.0)).filter(lambda t: t[0] % 2 == 0)
w2 = data
w3 = w.join(w2)
# w = data.map(lambda w: (w, w * 2)).filter(lambda t: t[0] % 2 == 0 and t[1] % 3 == 0)
w3.count()
# print(w2.count())



#
# f(1, 2)
# with open('/home/sabir/add.ll', 'w') as file:
#     llvm = f.inspect_llvm()
#     for k, v in llvm.items():
#         file.write(str(k))
#         file.write(v)
#         file.write('\n')
#
# @jit()
# def f(a, adict):
#     return a
#
# print(f(2, {1:2}))
# @cfunc("int64, float64(float64, float64)")
# @njit()
# def f(t):
#     return t + 4
#
#
# # f = njit(lambda t: t[0]+3)
# print(f(2.0))
# print(f(1 << 700))
# f([1.3], 2.3)
# f((2, 4, 3))
# print(f.inspect_types())
# f_llvm = f.inspect_llvm()
# for k, v in f_llvm.items():
#     # pass
#     # print(str(k))
#     mod = llvm.parse_assembly(v)
#     funcs = mod.functions
#     for fuk in funcs:
#         print(fuk)
