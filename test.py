import sys
import os
from time import sleep

import math

import numba
from numba import jit
from numba import njit
from numpy.matlib import rand

from blaze.blaze_context import BlazeContext
from timeit import default_timer as timer

#
sys.path.append(os.getcwd())
# #
bc = BlazeContext()
data1 = bc.collection([(v, rand(1, 1)[0] * 100) for v in range(0, 100)])
data2 = bc.collection([(v, rand(1, 1)[0] * 100) for v in range(0, 50)])

joined = data1.join(data2).map(lambda w: w).filter(lambda t: t[0] % 20 == 0).join(data1)
res = joined.collect()
print(res)
