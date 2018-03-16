import numpy as np
import pandas as pd
import pyspark as ps
import pyspark.sql.types as tp
# pylint: disable=no-name-in-module
# sabir 19 Feb 2018: col is created dynamically
from pyspark.sql.functions import rand, col

from jitq.utils import measure_time

MAX = 1 << 22
JOIN_1 = 1 << 7
JOIN_2 = 1 << 10

CONTEXT = ps.SparkContext(
    "local[1]",
    conf=ps.SparkConf().setExecutorEnv("memory", "14g").setExecutorEnv(
        "driver-memory", "4g"))
SQL_CONTEXT = ps.SQLContext(CONTEXT)

# generate MAX random floats
INPUT_ = SQL_CONTEXT.createDataFrame(
    pd.DataFrame(np.random.rand(MAX), columns=["c0"], dtype=float),
    schema=ps.sql.types.StructType(
        [ps.sql.types.StructField('c0', tp.DoubleType())]),
    verifySchema=False).cache()

INPUT_JOIN_1 = SQL_CONTEXT.createDataFrame(
    pd.DataFrame(
        np.random.randint(0, 2, size=(JOIN_1, 2)), columns=['c0', 'c1']),
    schema=tp.StructType([
        tp.StructField('c0', tp.LongType()),
        tp.StructField('c1', tp.LongType())
    ]),
    verifySchema=False)

INPUT_JOIN_2 = SQL_CONTEXT.createDataFrame(
    pd.DataFrame(
        np.random.randint(0, 2, size=(JOIN_2, 2)), columns=['c0', 'c2']),
    schema=tp.StructType([
        tp.StructField('c0', tp.LongType()),
        tp.StructField('c2', tp.LongType())
    ]),
    verifySchema=False)

INPUT_RBK = SQL_CONTEXT.range(0, MAX >> 1).select(
    "id", (rand() * 10).cast('long').alias("c0"),
    (rand() * 10).cast('long').alias("c1")).drop("id").cache()
INPUT_RBK.collect()

print("initialized")


def bench_sum():
    def run():
        INPUT_.agg(sum('c0')).collect()

    return run


def bench_map_filter():
    def run():
        INPUT_.withColumn('c1', col('c0') * 3 + 7)\
            .drop('c0').filter(col('c1') > 0.5)\
            .collect()

    return run


def bench_map():
    def run():
        INPUT_.withColumn('c1', col('c0') * 3 + 7).drop('c0').collect()

    return run


def bench_filter():
    def run():
        INPUT_.filter(col('c0') > 0.5).collect()

    return run


def bench_join():
    def run():
        return INPUT_JOIN_1.join(INPUT_JOIN_2, 'c0').collect()

    return run


def bench_map_filter_join():
    dataframe = INPUT_JOIN_2.withColumn(
        'c2',
        col('c0') * 3 + 7).filter(col('c0') > 0.5)
    dag = INPUT_JOIN_1.join(dataframe, 'c0')

    def run():
        return dag.collect()

    return run


def bench_reduce_by_key():
    dag = INPUT_RBK.groupBy('c0').agg(sum('c1'))

    def run():
        return dag.collect()

    return run


def bench_filter_sum():
    dag = INPUT_.filter(col('c0') > 0).agg(sum('c0'))

    def run():
        return dag.collect()

    return run


def run_benchmarks():
    print("benchmarking spark")
    print("--*--" * 10)

    print("time sum " + str(measure_time(bench_sum(), 3)))


if __name__ == '__main__':
    run_benchmarks()
