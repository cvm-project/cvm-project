import numpy as np
from jitq.jitq_context import JitqContext
from jitq.benchmarks.timer import measure_time

MAX = 1 << 27
JOIN_1 = 1 << 15
JOIN_2 = 1 << 20


def bench_sum():
    context = JitqContext()
    input_ = np.random.rand(MAX)
    dag = context.collection(input_)

    def run():
        return dag.reduce(lambda t1, t2: t1 + t2)

    return run


def bench_map_filter():
    context = JitqContext()
    input_ = np.random.rand(MAX)
    dag = context.collection(input_).map(lambda t: t * 3 + 7).filter(
        lambda t: t > 0.5)

    def run():
        return dag.collect()

    return run


def bench_map():
    context = JitqContext()
    input_ = np.random.rand(MAX)
    dag = context.collection(input_).map(lambda t: t * 3 + 7)

    def run():
        return dag.collect()

    return run


def bench_filter():
    context = JitqContext()
    input_ = np.random.rand(MAX)
    dag = context.collection(input_).filter(lambda t: t > 0.5)

    def run():
        return dag.collect()

    return run


def bench_join():
    context = JitqContext()

    input1 = np.random.randint(0, 1000, size=(JOIN_1, 2))
    input2 = np.random.randint(0, 1000, size=(JOIN_2, 2))

    in_rdd_1 = context.collection(input1)
    in_rdd_2 = context.collection(input2)
    dag = in_rdd_1.join(in_rdd_2)

    def run():
        return dag.collect()

    return run


def bench_map_filter_join():
    context = JitqContext()

    input1 = np.random.randint(0, 1000, size=(JOIN_1, 2))
    input2 = np.random.randint(0, 1000, size=JOIN_2)

    in_rdd_1 = context.collection(input1)
    in_rdd_2 = context.collection(input2).map(lambda t: (t, t * 3 + 7)).filter(
        lambda t: t[0] > 50.0 / 100)
    dag = in_rdd_1.join(in_rdd_2)

    def run():
        return dag.collect()

    return run


def bench_reduce_by_key():
    context = JitqContext()
    input_ = np.random.randint(0, 10, size=(MAX >> 1, 2))
    dag = context.collection(input_)

    def run():
        return dag.reduce_by_key(lambda t1, t2: t1 + t2).collect()

    return run


def bench_map_join():
    context = JitqContext()

    input1 = np.random.randint(0, 1000, size=(JOIN_1, 2))
    input2 = np.random.randint(0, 1000, size=JOIN_2)

    in_rdd_1 = context.collection(input1)
    in_rdd_2 = context.collection(input2).map(lambda t: (t, t * 3 + 7))
    dag = in_rdd_1.join(in_rdd_2)

    def run():
        return dag.collect()

    return run


def bench_map_reduce_by_key():
    context = JitqContext()
    input_ = np.random.randint(0, 10, size=(MAX >> 1, 2))
    dag = context.collection(input_)

    def run():
        return dag.map(lambda t: (t[0], t[1] * 3 + 7)).reduce_by_key(
            lambda t1, t2: t1 + t2).collect()

    return run


def run_benchmarks():
    print("benchmarking jitq")
    print("--*--" * 10)

    print("time map reduce by key " +
          str(measure_time(bench_map_reduce_by_key())))


if __name__ == '__main__':
    run_benchmarks()
