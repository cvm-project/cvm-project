import os
from blaze.blaze_context import *
import numpy as np
import random

# sys.path.insert(0, "/home/sabir/projects/")
from numba import njit

input_ = [(1, 2), (5, 4)]


def func(t1, t2):
    return t1 + t2


# bc = BlazeContext()
#
# print(bc.collection(input_).map(lambda *t: (t[0:2], t[1:])).collect())

# data2 = bc.collection([(1, 33), (0, 0)]).map(lambda t: (t[0]*3, t[0])).filter(lambda t: t[1] > -1).map(lambda t: (t[1], t[0]))
# print(bc.collection(input_).map(func).filter(lambda t: t[0] == 1).join(data2).collect())

bc = BlazeContext()
data2 = bc.collection([(1, 33), (0, 0)]).map(lambda t: (t[0] * 3, t[0])).filter(lambda t: t[1] > -1).map(
    lambda t: (t[1], t[0]))
print(bc.collection(input_).map(lambda t: (t[1], t[0])).filter(lambda t: t[0] == 1).join(data2).collect())
# bc.collection(input_).map(func).reduce_by_key(lambda t1, t2: (t1[0] + t2[0], t1[1])).collect()
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
# def callee(t):
#     print(t)
#     return 17
#
#
# @njit()
# def caller(a):
#     return [i for i in a]
#
#
# caller([1,2,3])
# # # f([1, 2, 3])
# # # # print(f(2.0))
# # # # print(f(1 << 700))
# # # # f([1.3], 2.3)
# # # # f((2, 4, 3))
# # # # print(f.inspect_types())
# f_llvm = f.inspect_llvm()
# for k, v in f_llvm.items():
#     print(str(v))
# mod = llvm.parse_assembly(v)
#     funcs = mod.functions
#     for fuk in funcs:
#         print(fuk)
