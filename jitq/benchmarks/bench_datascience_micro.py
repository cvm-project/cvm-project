import pandas as pd
import numpy as np
from numba import njit

from jitq.benchmarks.timer import Timer
from jitq.jitq_context import JitqContext

SIZE = 1 << 27
DATA = np.random.rand(SIZE)
print("data generated ")


def bench_numpy():
    timer_ = Timer()
    timer_.start()
    DATA[DATA > 0].sum()
    timer_.end()
    print("numpy time: " + timer_.diff())


def bench_numba():
    @njit(fastmath=False, cache=True)
    def numba(data_):
        acc = 0
        for elem in data_:
            if elem > 0:
                acc += elem
        return acc

    timer_ = Timer()
    timer_.start()
    numba(DATA)
    timer_.end()
    print("numba time: " + timer_.diff())


def bench_jitq():
    context = JitqContext()
    timer_ = Timer()
    timer_.start()
    context.collection(DATA).filter(lambda x: x > 0).reduce(lambda a, b: a + b)
    timer_.end()

    print("jitq time: " + timer_.diff())


def bench_pandas():
    timer_ = Timer()
    pd_df = pd.DataFrame(DATA, dtype=float)
    timer_.start()
    pd_df[pd_df > 0].sum()
    timer_.end()
    print("pandas time: " + timer_.diff())
