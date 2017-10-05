import pandas as pd
import numpy as np
import os
import sys

sys.path.insert(0, os.environ['BLAZEPATH'])

from blaze.benchmarks.timer import Timer
from blaze.blaze_context import BlazeContext
from numba import njit

size = 1 << 28
data = np.random.rand(size)
print("data generated ")
for i in range(3):
    t = Timer()

    t.start()
    data[data > 0].sum()
    t.end()

    print("numpy time: " + t.diff())


    @njit(fastmath=True)
    def np_numba(data_):
        return data_[data_ > 0].sum()


    t.start()
    np_numba(data)
    t.end()

    print("numpy plus numba time: " + t.diff())


    @njit(fastmath=True, cache=True)
    def numba(data_):
        d = 0
        for el in data_:
            if el > 0:
                d += el
        return d


    t.start()
    numba(data)
    t.end()

    print("numba time: " + t.diff())

    bc = BlazeContext()
    t.start()
    bc.numpy_array(data).filter(lambda x: x > 0).reduce(lambda a, b: a + b)
    t.end()

    print("blaze time: " + t.diff())

    # pandas
    pd_df = pd.DataFrame(data, columns=['c0'])
    t.start()
    pd_res = pd_df.query('c0>0').sum()
    t.end()

    print("pandas time: " + t.diff())
