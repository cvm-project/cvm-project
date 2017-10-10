import numpy as np
import sys

sys.path.insert(0, "/home/sabir/projects/blaze")
import pandas as pd
import pyspark as ps
from pyspark.sql.functions import sum, col, rand
from blaze.benchmarks.timer import timer
from operator import add
import pyspark.sql.types as tp

MAX = 1 << 22
# MAX = 1 << 2
join1 = 1 << 7
# join1 = 1 << 2
join2 = 1 << 10
# join2 = 1 << 4

sparkContext = ps.SparkContext("local[1]",
                               conf=ps.SparkConf().setExecutorEnv("memory", "6g").setExecutorEnv("driver-memory",
                                                                                                 "10g"))
sqlContext = ps.SQLContext(sparkContext)

# input_ = sqlContext.createDataFrame(pd.DataFrame(np.random.rand(MAX), columns=["c0"], dtype=float),
#                                     schema=ps.sql.types.StructType([ps.sql.types.StructField('c0', tp.DoubleType())]),
#                                     verifySchema=False)

input_ = sqlContext.range(0, MAX).select(rand().alias("c0")).cache()
input_.collect()

# input_join1 = sqlContext.createDataFrame(
#     pd.DataFrame(np.random.randint(0, 2, size=(join1, 2)), columns=['c0', 'c1']),
#     schema=tp.StructType([tp.StructField('c0', tp.LongType()), tp.StructField('c1', tp.LongType())]),
#     verifySchema=False)

# input_join2 = sqlContext.createDataFrame(
#     pd.DataFrame(np.random.randint(0, 2, size=(join2, 2)), columns=['c0', 'c2']),
#     schema=tp.StructType([tp.StructField('c0', tp.LongType()), tp.StructField('c2', tp.LongType())]),
#     verifySchema=False)

print("initialized")


def bench_sum():
    def run():
        input_.agg(sum('c0')).collect()

    return run


def bench_map_filter():
    def run():
        input_.withColumn('c1', col('c0') * 3 + 7).drop('c0').filter(col('c1') > 0.5).collect()

    return run


def bench_map():
    def run():
        input_.withColumn('c1    ', col('c0') * 3 + 7).drop('c0').collect()

    return run


def bench_filter():
    def run():
        input_.filter(col('c0') > 0.5).collect()

    return run


def bench_join():
    def run():
        return input_join1.join(input_join2, 'c0').collect()

    return run


input_join2_ = sqlContext.range(0, join2).select((rand() * 1000).cast('long').alias("c0")).cache()
input_join2_.collect()


def bench_map_filter_join():
    d2 = input_join2_.withColumn('c2', col('c0') * 3 + 7).filter(col('c0') > 0.5)
    dag = input_join1.join(d2, 'c0')

    def run():
        return dag.collect()

    return run


input_reduce_by_key = sqlContext.range(0, MAX >> 1).select("id", (rand() * 10).cast('long').alias("c0"),
                                                           (rand() * 10).cast('long').alias("c1")).drop("id").cache()
input_reduce_by_key.collect()


def bench_reduce_by_key():
    # input__ = sqlContext.createDataFrame(
    #     pd.DataFrame(np.random.randint(0, 10, size=(MAX >> 1, 2)), columns=["c0", 'c1']),
    #     schema=tp.StructType([tp.StructField('c0', tp.LongType()), tp.StructField('c1', tp.LongType())]),
    #     verifySchema=False
    # )
    dag = input_reduce_by_key.groupBy('c0').agg(sum('c1'))

    def run():
        return dag.collect()

    return run


def bench_filter_sum():
    dag = input_.filter(col('c0') > 0).agg(sum('c0'))

    def run():
        return dag.collect()

    return run


print("benchmarking spark")
print("--*--" * 10)

# t = timer(bench_filter(), 3)
# print("time filter " + str(t))
#
# t = timer(bench_sum(), 3)
# print("time sum " + str(t))
#
# t = timer(bench_map())
# print("time map " + str(t))
#
# t = timer(bench_map_filter())
# print("time map_filter " + str(t))



t = timer(bench_filter_sum())
print("time filter sum " + str(t))

# sqlContext.clearCache()
#
# input_join1 = sqlContext.range(0, join1).select((rand() * 1000).cast('long').alias("c0"),
#                                                 (rand() * 1000).cast('long').alias("c1")).cache()
# input_join2 = sqlContext.range(0, join2).select((rand() * 1000).cast('long').alias("c0"),
#                                                 (rand() * 1000).cast('long').alias("c1")).cache()
#
# t = timer(bench_join())
# print("time join " + str(t))
#
# sqlContext.clearCache()
# t = timer(bench_map_filter_join())
# print("time map filter join " + str(t))
#
# sqlContext.clearCache()
# t = timer(bench_reduce_by_key())
# print("time reduce by key " + str(t))
