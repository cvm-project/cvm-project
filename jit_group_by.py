import sys
import os

from numba import jit

from jitq.jitq_context import JitqContext
from timeit import default_timer as timer

sys.path.append(os.getcwd())

start = timer()
bc = JitqContext()
tf1 = bc.text_file("jitq/example_data/xaa")


gr = tf1.flat_map(lambda line: [i for i in range(0, 100)]) \
    .map(lambda w: (1, 1)) \
    .group_by_key()

gr.collect()
from jitq.operations import group_by_key

# print(group_by_key.inspect_llvm())
