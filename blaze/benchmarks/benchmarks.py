import os
import math
from functools import reduce
import numpy as np
import sys
sys.path.insert(0, "/home/sabir/projects/blaze")

from blaze.blaze_context import BlazeContext
from blaze.benchmarks.timer import timer

MAX = 1 << 29

def bench_sum():
    bc = BlazeContext()
    input_ = np.random.rand(MAX)
    dag = bc.numpy_array(input_)

    def run():
        return dag.reduce(lambda t1, t2: t1 + t2)

    return run


# sum

t_sum = timer(bench_sum())
print("time sum " + str(t_sum))
