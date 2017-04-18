import sys
import os
from time import sleep

import math
from numba import jit

from blaze.blaze_context import BlazeContext
from timeit import default_timer as timer

sys.path.append(os.getcwd())

bc = BlazeContext()
data = bc.collection([i for i in range(0, 10000000)])


def long_running_function(v):
    return v, math.sqrt(v)


gr = data.map(long_running_function) \
    .filter(lambda w: w[0] % 2)

start = timer()
r1 = gr.collect()
end = timer()
print('ELAPSED %f' % (end - start))

faster_gr = data.filter(lambda w: w % 2) \
    .map(long_running_function)

start = timer()
r2 = faster_gr.collect()
end = timer()
print('ELAPSED %f' % (end - start))
assert r1 == r2
