import sys
import os
from time import sleep

import math

from blaze.blaze_context import BlazeContext
from numba import cfunc

sys.path.insert(0, "/home/sabir/projects/")
print(sys.path)
from numba import njit
import llvmlite.binding as llvm
import numba as nb

# #
bc = BlazeContext()
data = bc.collection(list([i for i in range(0, 50)]))



# os.environ["NUMBA_DUMP_IR"] = '1'
# os.environ["NUMBA_DUMP_ASSEMBLY"] = '1'
os.environ["NUMBA_DUMP_OPTIMIZED"] = '1'


# os.environ["NUMBA_DUMP_LLVM"] = '1'

w = data.map(lambda w: (w, w * 2)).filter(lambda t: t[0] % 2 == 0 and t[1] % 3 == 0)
# w = data.map(lambda w: (w, w * 2)).filter(lambda t: t[0] % 2 == 0 and t[1] % 3 == 0)
print(w.collect())

# @njit()
# def my_g():
#     for i in [0, 10]:
#         yield i

# alist = numba.types.Array([1,2])

# op = njit(open)
# @njit()
# def f():
#     for a in open('dsf'):
#         print (a)

# f()
# def f():
#     @njit()
#     def my_g():
#         for i in [0, 10]:
#             yield i
#
#     @njit()
#     def bar():
#         for a in my_g():
#             print (a)
#
#     bar()

# f()
#
# list_ = [1,2]
# @jit()
# def f(a, b):
#     file = open('/home/sabir/add.ll')
#     print(file)
#     file.close()
#     return a + b
#
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
