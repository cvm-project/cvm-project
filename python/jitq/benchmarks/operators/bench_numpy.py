import numpy as np

from jitq.utils import measure_time

MAX = 1 << 27


def bench_sum():
    input_ = np.random.rand(MAX)

    def run():
        return input_.sum()

    return run


def bench_map_filter():
    input_ = np.random.rand(MAX)

    def run():
        result = input_ + 3
        result *= 7
        return result[result > 0.5]

    return run


def bench_map():
    input_ = np.random.rand(MAX)

    def run():
        result = input_ + 3
        result *= 7
        return result

    return run


def bench_filter():
    input_ = np.random.rand(MAX)

    def run():
        return input_[input_ > 0.5]

    return run


def run_benchmarks():
    print("benchmarking numpy")
    print("--*--" * 10)

    print("time map_filter " + str(measure_time(bench_map_filter())))


if __name__ == '__main__':
    run_benchmarks()
