import sys
import os
from time import sleep

import math

import numba
from numba import jit
from numba import njit

# from blaze.blaze_context import BlazeContext
# from timeit import default_timer as timer
#
# sys.path.append(os.getcwd())
# #
# bc = BlazeContext()
# data = bc.collection(list([i for i in range(0, 5)]))
#
# w = data.map(lambda w: w * 1).filter(lambda w: w % 2 == 0).flat_map(lambda w: [1, 10, 12])
# print(w.collect())

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

list_ = [1,2]
@njit()
def f(a, b):
    return a+b
f(1,2)
print(f.signatures)
# print(type(b))
# list_[0]
# f(1)
# with open('/home/sabir/add.ll', 'w') as file:
#     llvm = f.inspect_llvm()
#     for k, v in llvm.items():
#         file.write(str(k))
#         file.write(v)
#         file.write('\n')
