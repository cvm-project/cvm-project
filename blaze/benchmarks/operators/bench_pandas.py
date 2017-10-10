import os
import math
from functools import reduce
import numpy as np
import sys

sys.path.insert(0, "/home/sabir/projects/blaze")

import pandas as pd
from blaze.benchmarks.timer import timer

MAX = 1 << 27
join1 = 1 << 15
join2 = 1 << 20


def bench_sum():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        return input_.sum()

    return run


def bench_map_filter():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        d = input_
        d = d * 3 + 7
        return d[d > 0.5]

    return run


def bench_map():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        return input_ * 3 + 7

    return run


def bench_filter():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        return input_[input_ > 0.5]

    return run


def bench_join():
    input1 = pd.DataFrame(np.random.randint(0, 1000, size=(join1, 2)))
    input2 = pd.DataFrame(np.random.randint(0, 1000, size=(join2, 2)))

    def run():
        return pd.merge(input1, input2, on=[0], sort=False)

    return run


def bench_map_filter_join():
    input1 = pd.DataFrame(np.random.randint(0, 1000, size=(join1, 2)))
    input2 = pd.DataFrame(np.random.randint(0, 1000, size=(join2, 2)))

    def run():
        d2 = input2 * 3 + 7
        return pd.merge(input1, d2[d2 > 0.5], on=[0], sort=False)

    return run


def bench_reduce_by_key():
    input_ = pd.DataFrame(np.random.randint(0, 10, size=(MAX >> 1, 2)))

    def run():
        input_.groupby([0]).sum()

    return run


def bench_map_reduce_by_key():
    input_ = pd.DataFrame(np.random.randint(0, 10, size=(MAX >> 1, 2)))

    def run():
        d = input_
        d[1] = d[1] * 3 + 7
        return d.groupby([0]).sum()

    return run


def bench_map_reduce_by_key_map():
    input_ = pd.DataFrame(np.random.randint(0, 10, size=(MAX >> 1, 2)))

    def run():
        d = input_
        d = d * 3 + 7
        g = d.groupby([0]).sum()
        g['bitmap'] = g > 0
        return

    return run


def bench_map_join():
    input1 = pd.DataFrame(np.random.randint(0, 1000, size=(join1, 2)))
    input2 = pd.DataFrame(np.random.randint(0, 1000, size=(join2, 2)))

    def run():
        input2[1] = input2[1] * 3 + 7
        return pd.merge(input1, input2, on=[0], sort=False)

    return run


print("benchmarking pandas")
print("--*--" * 10)

# t = timer(bench_map())
# print("time map " + str(t))
#
# t = timer(bench_filter())
# print("time filter " + str(t))
#
# t_sum = timer(bench_sum())
# print("time sum " + str(t_sum))

# map_filter
# t_sum = timer(bench_map_filter())
# print("time map_filter " + str(t_sum))

# join
# t_sum = timer(bench_join())
# print("time join " + str(t_sum))
#
# t = timer(bench_reduce_by_key())
# print("time reduce by key " + str(t))

# t = timer(bench_map_filter_join())
# print("time map filter join " + str(t))

t = timer(bench_map_join())
print("time map join  " + str(t))
#
# t = timer(bench_map_reduce_by_key())
# print("time map rbk  " + str(t))
