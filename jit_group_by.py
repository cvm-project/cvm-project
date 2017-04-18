import sys
import os

from numba import jit

from blaze.blaze_context import BlazeContext
from timeit import default_timer as timer

sys.path.append(os.getcwd())

start = timer()
bc = BlazeContext()
tf1 = bc.text_file("blaze/example_data/xaa")


gr = tf1.flat_map(lambda line: [i for i in range(0, 100)]) \
    .map(lambda w: (1, 1)) \
    .group_by_key()

gr.collect()
from blaze.operations import group_by_key

# print(group_by_key.inspect_llvm())
