import os
import math
from functools import reduce
import numpy as np
import sys

sys.path.insert(0, "/home/sabir/projects/blaze")

from blaze.blaze_context import BlazeContext
from blaze.benchmarks.timer import timer

MAX = 1 << 27
join1 = 1 << 15
join2 = 1 << 20


def bench_sum():
    bc = BlazeContext()
    input_ = np.random.rand(MAX)
    dag = bc.collection(input_)

    def run():
        return dag.reduce(lambda t1, t2: t1 + t2)

    return run


def bench_map_filter():
    bc = BlazeContext()
    input_ = np.random.rand(MAX)
    dag = bc.collection(input_).map(lambda t: t * 3 + 7).filter(lambda t: t > 0.5)

    def run():
        return dag.collect()

    return run


def bench_map():
    bc = BlazeContext()
    input_ = np.random.rand(MAX)
    dag = bc.collection(input_).map(lambda t: t * 3 + 7)

    def run():
        return dag.collect()

    return run


def bench_filter():
    bc = BlazeContext()
    input_ = np.random.rand(MAX)
    dag = bc.collection(input_).filter(lambda t: t > 0.5)

    def run():
        return dag.collect()

    return run


def bench_join():
    bc = BlazeContext()

    input1 = np.random.randint(0, 1000, size=(join1, 2))
    input2 = np.random.randint(0, 1000, size=(join2, 2))

    inRDD1 = bc.collection(input1)
    inRDD2 = bc.collection(input2)
    dag = inRDD1.join(inRDD2)

    def run():
        return dag.collect()

    return run


def bench_map_filter_join():
    bc = BlazeContext()

    input1 = np.random.randint(0, 1000, size=(join1, 2))
    input2 = np.random.randint(0, 1000, size=join2)

    inRDD1 = bc.collection(input1)
    inRDD2 = bc.collection(input2).map(lambda t: (t, t * 3 + 7)).filter(lambda t: t[0] > 50.0 / 100)
    dag = inRDD1.join(inRDD2)

    def run():
        return dag.collect()

    return run


def bench_reduce_by_key():
    bc = BlazeContext()
    input_ = np.random.randint(0, 10, size=(MAX >> 1, 2))
    dag = bc.collection(input_)

    def run():
        return dag.reduce_by_key(lambda t1, t2: t1 + t2).collect()

    return run


def bench_map_join():
    bc = BlazeContext()

    input1 = np.random.randint(0, 1000, size=(join1, 2))
    input2 = np.random.randint(0, 1000, size=join2)

    inRDD1 = bc.collection(input1)
    inRDD2 = bc.collection(input2).map(lambda t: (t, t * 3 + 7))
    dag = inRDD1.join(inRDD2)

    def run():
        return dag.collect()

    return run


def bench_map_reduce_by_key():
    bc = BlazeContext()
    input_ = np.random.randint(0, 10, size=(MAX >> 1, 2))
    dag = bc.collection(input_)

    def run():
        return dag.map(lambda t: (t[0], t[1] * 3 + 7)).reduce_by_key(lambda t1, t2: t1 + t2).collect()

    return run


print("benchmarking our")
print("--*--" * 10)

# t = timer(bench_sum())
# print("time sum " + str(t))
#
# t = timer(bench_map())
# print("time map " + str(t))
#
# t = timer(bench_filter())
# print("time filter " + str(t))
#
#
# #
# t = timer(bench_join())
# print("time join " + str(t))
#
# t = timer(bench_reduce_by_key())
# print("time reduce by key " + str(t))

# t = timer(bench_map_filter())
# print("time map_filter " + str(t))
#
# t = timer(bench_map_filter_join())
# print("time map filter join " + str(t))

t = timer(bench_map_reduce_by_key())
print("time map reduce by key " + str(t))

# t = timer(bench_map_join())
# print("time map join  " + str(t))
