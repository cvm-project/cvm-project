import numpy as np
import pandas as pd

from jitq.utils import measure_time

MAX = 1 << 27
JOIN_1 = 1 << 15
JOIN_2 = 1 << 20


def bench_sum():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        return input_.sum()

    return run


def bench_map_filter():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        result = input_
        result = result * 3 + 7
        return result[result > 0.5]

    return run


def bench_map():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        return input_ * 3 + 7

    return run


def bench_filter():
    input_ = pd.DataFrame(np.random.rand(MAX))

    def run():
        return input_[input_ > 0.5]

    return run


def bench_join():
    input1 = pd.DataFrame(np.random.randint(0, 1000, size=(JOIN_1, 2)))
    input2 = pd.DataFrame(np.random.randint(0, 1000, size=(JOIN_2, 2)))

    def run():
        return pd.merge(input1, input2, on=[0], sort=False)

    return run


def bench_map_filter_join():
    input1 = pd.DataFrame(np.random.randint(0, 1000, size=(JOIN_1, 2)))
    input2 = pd.DataFrame(np.random.randint(0, 1000, size=(JOIN_2, 2)))

    def run():
        result = input2 * 3 + 7
        return pd.merge(input1, result[result > 0.5], on=[0], sort=False)

    return run


def bench_reduce_by_key():
    input_ = pd.DataFrame(np.random.randint(0, 10, size=(MAX >> 1, 2)))

    def run():
        input_.groupby([0]).sum()

    return run


def bench_map_reduce_by_key():
    input_ = pd.DataFrame(np.random.randint(0, 10, size=(MAX >> 1, 2)))

    def run():
        result = input_
        result[1] = result[1] * 3 + 7
        return result.groupby([0]).sum()

    return run


def bench_map_reduce_by_key_map():
    input_ = pd.DataFrame(np.random.randint(0, 10, size=(MAX >> 1, 2)))

    def run():
        result = input_
        result = result * 3 + 7
        grouped = result.groupby([0]).sum()
        grouped['bitmap'] = grouped > 0
        return

    return run


def bench_map_join():
    input1 = pd.DataFrame(np.random.randint(0, 1000, size=(JOIN_1, 2)))
    input2 = pd.DataFrame(np.random.randint(0, 1000, size=(JOIN_2, 2)))

    def run():
        input2[1] = input2[1] * 3 + 7
        return pd.merge(input1, input2, on=[0], sort=False)

    return run


def run_benchmarks():
    print("benchmarking pandas")
    print("--*--" * 10)

    print("time map join  " + str(measure_time(bench_map_join())))


if __name__ == '__main__':
    run_benchmarks()
