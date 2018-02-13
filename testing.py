import os

from numba import njit
import numpy as np
from jitq.jitq_context import JitqContext

# os.environ["NUMBA_DUMP_IR"] = '1'
# os.environ["NUMBA_DUMP_ASSEMBLY"] = '1'
os.environ["NUMBA_DUMP_OPTIMIZED"] = '1'


# @njit()
def id_(ls):
    d = 0
    for sh in ls:
        d += sh
    return d


bc = JitqContext()
bc.numpy_array(np.array([[1, 2], [2, 3]])).map(id_).collect()
