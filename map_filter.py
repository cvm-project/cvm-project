import os
from blaze.blaze_context import *
import numpy as np
import random

# sys.path.insert(0, "/home/sabir/projects/")
from numba import njit

# input = [(1, 2), (5, 6.2)]


# data = collection(input)


# os.environ["NUMBA_DUMP_IR"] = '1'
# os.environ["NUMBA_DUMP_ASSEMBLY"] = '1'
# os.environ["NUMBA_DUMP_OPTIMIZED"] = '1'


# os.environ["NUMBA_DUMP_LLVM"] = '1'

def func(a):
    return a + 1, a * 300


# func = lambda t: t[3]
# print(input)


# w = data.map(lambda w: (w, w * 2)).filter(lambda t: t[0] % 2 == 0 and t[1] % 3 == 0)

import time
def scope():
# input = [(r, r) for r in range(1, 10000000)]
    input = np.ones(500000000)
    bc = BlazeContext
    data = bc.numpy_array(input)
    w = data.map(lambda t: t)
    res = w.collect()
    print(res)

scope()

time.sleep(5)
print("after")
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
