import os
import time

import math
from functools import reduce

from numba import njit
import numpy as np


def mean(l):
    return sum(l) / len(l)


def timer(func, max_rep=3):
    res = []
    for r in range(0, max_rep):
        t1 = time.perf_counter()
        resu = func()
        t2 = time.perf_counter()
        res.append(t2 - t1)
        print(resu)
    return mean(res)


# os.environ["NUMBA_DUMP_IR"] = '1'
# os.environ["NUMBA_DUMP_OPTIMIZED"] = '1'
# os.environ["NUMBA_DUMP_ASSEMBLY"] = '1'

max = 1 << 27

udf = lambda a, b: a + b



def f():
    acc = 0.0
    v0 = 0.0
    while v0 < max:
        acc = udf(acc, v0)
        v0 += 1
    return acc


# res = njit(f)()
# print(res)
#
# t_numba = timer(njit(f))
# print("numba " + str(t_numba))

def ma_gen():
    v0 = 0.0
    while v0 < max:
        yield v0
        v0 += 1.0

# t_builtin_sum = timer(lambda: sum(ma_gen()))
# print("built in sum " + str(t_builtin_sum))
#
# t_builtin_reduce = timer(lambda: reduce(lambda a, b: a + b, range(0, max)))
# print("built in reduce " + str(t_builtin_reduce))

# t_python = timer(f)
# print("pure python " + str(t_python))

map_udf = lambda a: (a, a*3 + 7)
filter_udf = lambda t: t[0] % 2 == 0
n_udf = njit(udf)

# def func2():
#     v0 = 0.0
#     acc = 0.0
#     while v0 < max:
#         t1 = map_udf(v0)
#         v0 += 1
#         if not filter_udf(t1):
#             continue
#         acc = n_udf(acc, t1[0])
#     return acc

# t_numba = timer(njit(func2))
# print("time numba " + str(t_numba))

def python_b():
    return reduce(udf, filter(filter_udf, map(map_udf, ma_gen())))
#
t_python_builtin = timer(python_b)
print("time python builtin " + str(t_python_builtin))
#
#
#
# map_udf2 = lambda a: (a, a*3 + 7)
# filter_udf2 = lambda t: t[0] % 2 == 0
#
# def python():
#     v0 = 0.0
#     acc = 0.0
#     while v0 < max:
#         t1 = map_udf2(v0)
#         v0 += 1
#         if not filter_udf2(t1):
#             continue
#         acc = udf(acc, t1[0])
#     return acc

# t_python = timer(python)
# print("time python" + str(t_python))

