import numpy as np
import sys

sys.path.insert(0, "/home/sabir/projects/jitq")

from jitq.benchmarks.timer import timer

MAX = 1 << 27


def bench_sum():
    input_ = np.random.rand(MAX)

    def run():
        return input_.sum()

    return run


def bench_map_filter():
    input_ = np.random.rand(MAX)

    def run():
        d = input_ + 3
        d *= 7
        return d[d > 0.5]

    return run


def bench_map():
    input_ = np.random.rand(MAX)

    def run():
        d = input_ + 3
        d *= 7
        # return np.add(input_ * 3, 7, out=input_)
        return d

    return run


def bench_filter():
    input_ = np.random.rand(MAX)

    def run():
        return input_[input_ > 0.5]

    return run


print("benchmarking numpy")
print("--*--" * 10)

# t = timer(bench_map())
# print("time map " + str(t))
#
# t = timer(bench_filter(), 3)
# print("time filter " + str(t))

# t = timer(bench_sum(), 3)
# print("time sum " + str(t))

t = timer(bench_map_filter())
print("time map_filter " + str(t))
