#!/usr/bin/env python3

import unittest
from itertools import groupby

from functools import reduce
from operator import itemgetter

import numpy as np
import pandas

from jitq.jitq_context import JitqContext


# Disable "too-many-public-methods": this is not an interface, but a collection
# of test cases, which may be long.
# pylint: disable=R0904
class TestOperators(unittest.TestCase):
    #
    # Collection
    #

    def test_collection_index(self):
        context = JitqContext()
        res = context.collection(
            [i**2 for i in range(10)], add_index=True).collect()
        self.assertListEqual(res.astuplelist(), [(i, i**2) for i in range(10)])

    def test_collection_tuple(self):
        context = JitqContext()
        collection = [(i, 2 * i) for i in range(10)]
        res = context.collection(collection).collect()
        self.assertListEqual(res.astuplelist(), collection)

    def test_collection_array(self):
        context = JitqContext()
        collection = [(i, 2 * i) for i in range(10)]
        res = context.collection(np.array(collection)).collect()
        self.assertListEqual(res.astuplelist(), collection)

    def test_collection_array_index(self):
        context = JitqContext()
        collection = [(i, 2 * i) for i in range(10)]
        res = context.collection(
            np.array(collection), add_index=True).collect()
        ref = [(i, ) + r for i, r in enumerate(collection)]
        self.assertListEqual(res.astuplelist(), ref)

    def test_pandas_dataframe(self):
        context = JitqContext()
        array = pandas.DataFrame([(i, 2 * i) for i in range(10)])
        res = context.collection(array).collect()
        truth = [tuple(r) for r in array.values.tolist()]
        self.assertListEqual(truth, res.astuplelist())

    #
    # Range
    #

    def test_range_simple(self):
        context = JitqContext()
        res = context.range_(0, 10).collect()
        self.assertListEqual(res.astuplelist(), list(range(10)))

    def test_range_start(self):
        context = JitqContext()
        res = context.range_(10, 20).collect()
        self.assertListEqual(res.astuplelist(), list(range(10, 20)))

    def test_range_step(self):
        context = JitqContext()
        res = context.range_(10, 20, 2).collect()
        self.assertListEqual(res.astuplelist(), list(range(10, 20, 2)))

    def test_range_float(self):
        context = JitqContext()
        res = context.range_(10.5, 20.0, 2.0).collect()
        self.assertListEqual(res.astuplelist(),
                             list(np.arange(10.5, 20.0, 2.0)))

    #
    # Join
    #

    def test_join_overlap(self):
        jitq_context = JitqContext()
        input_1 = [(r, r * 10) for r in range(10)]
        input_2 = [(r, r * 13, r + 100) for r in range(5, 15)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        joined = data1.join(data2)
        res = joined.collect()
        truth = [(r, r * 10, r * 13, r + 100) for r in range(5, 10)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_join_repeated_keys(self):
        jitq_context = JitqContext()
        input_1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        joined = data1.join(data2)
        res = joined.collect()
        truth = [(1, 2, 22, 33), (1, 3, 22, 33), (1, 2, 44, 55),
                 (1, 3, 44, 55), (2, 4, 33, 44), (2, 6, 33, 44)]
        self.assertListEqual(res.astuplelist(), truth)

    #
    # Filter
    #

    def test_filter_count(self):
        jitq_context = JitqContext()
        data = jitq_context.collection(range(0,
                                             10)).filter(lambda w: w % 2 == 0)
        res = data.count()
        self.assertEqual(res,
                         len(list(filter(lambda w: w % 2 == 0, range(0, 10)))))

    def test_filter_collect(self):
        jitq_context = JitqContext()
        data = jitq_context.collection(range(0,
                                             10)).filter(lambda w: w % 2 == 0)
        res = data.collect()
        self.assertListEqual(res.astuplelist(),
                             list(filter(lambda w: w % 2 == 0, range(0, 10))))

    #
    # Map
    #

    def test_map_collect(self):
        jitq_context = JitqContext()
        data = jitq_context.collection(range(
            0, 10)).map(lambda t: (t, (t, t * 10)))
        res = data.collect()
        self.assertListEqual(res.astuplelist(),
                             [(t, t, t * 10) for t in range(0, 10)])

    def test_map_count(self):
        jitq_context = JitqContext()
        data = jitq_context.collection(range(0, 10)).map(lambda t: (t, t * 10))
        res = data.count()
        self.assertEqual(res, 10)

    #
    # Reduce
    #

    def test_reduce_sum(self):
        jitq_context = JitqContext()
        inpt = range(0, 10)
        data = jitq_context.collection(inpt).reduce(
            lambda tuple_1, tuple_2: tuple_1 + tuple_2)
        self.assertEqual(data, sum(inpt))

    def test_reduce_tuple(self):
        jitq_context = JitqContext()
        inpt = [(r, r * 10) for r in range(0, 10)]
        res = jitq_context.collection(inpt).reduce(
            lambda tuple_1, tuple_2: (
                tuple_1[0] + tuple_2[0], tuple_1[1] + tuple_2[1])
        )
        res = tuple(res)
        truth = (sum(map(lambda t: t[0], inpt)), sum(
            map(lambda t: t[1], inpt)))
        self.assertTupleEqual(res, truth)

    #
    # Reduce by key
    #

    def test_reduce_by_key(self):
        jitq_context = JitqContext()
        inpt = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]
        data = jitq_context.collection(inpt).reduce_by_key(
            lambda tuple_1, tuple_2: tuple_1 + tuple_2).collect()
        truth = {(0, 2), (1, 3)}
        self.assertSetEqual(set(data.astuples()), truth)

    def test_reduce_by_key_tuple(self):
        jitq_context = JitqContext()
        input_ = [(0, 1, 2), (1, 1, 2), (1, 1, 2), (0, 1, 2), (1, 1, 2)]
        data = jitq_context.collection(input_).reduce_by_key(
            lambda tuple_1, tuple_2: (
                tuple_1[0] + tuple_2[0], tuple_1[1] + tuple_2[1])
        ).collect()
        truth = [(0, 2, 4), (1, 3, 6)]
        self.assertEqual(sorted(data.astuples()), sorted(truth))

    def test_reduce_by_key_grouped(self):
        jitq_context = JitqContext()
        input_ = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]

        def reduce_func(tuple_1, tuple_2):
            return (tuple_1[0] + tuple_2[0], tuple_1[1] + tuple_2[1],
                    tuple_1[2] + tuple_2[2], tuple_1[3] + tuple_2[3],
                    tuple_1[4] + tuple_2[4])

        enumerated_input = [(i,) + t for i, t in enumerate(input_)]
        cart = [
            tuple_1 + tuple_2 for tuple_1 in enumerated_input
            for tuple_2 in enumerated_input
        ]
        truth = [reduce(lambda tuple_1,
                        tuple_2: (tuple_1[0],
                                  ) + reduce_func(tuple_1[1:],
                                                  tuple_2[1:]),
                        group) for _,
                 group in groupby(sorted(cart),
                                  key=itemgetter(0))]

        data = jitq_context.collection(input_, add_index=True).cartesian(
            jitq_context.collection(input_, add_index=True)) \
            .flatten().reduce_by_key(reduce_func).collect()

        self.assertEqual(sorted(data.astuples()), sorted(truth))

    #
    # Cartesian
    #

    def test_cartesian_count(self):
        jitq_context = JitqContext()
        input_1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        joined = data1.cartesian(data2)
        res = joined.count()
        truth = 20
        self.assertEqual(res, truth)

    def test_cartesian_collect(self):
        jitq_context = JitqContext()
        input_1 = [(1, 2), (1, 3)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        joined = data1.cartesian(data2)
        res = joined.collect()
        truth = {(1, 2, 1, 22, 33), (1, 2, 1, 44, 55), (1, 2, 8, 66, 77),
                 (1, 3, 1, 22, 33), (1, 3, 1, 44, 55), (1, 3, 8, 66, 77)}
        self.assertSetEqual(set(res.astuples()), truth)

    #
    # Cache
    #

    def test_cache_map1(self):
        for _ in range(2):
            context = JitqContext()
            res = context.collection(range(0,
                                           10)).map(lambda t: t + 1).collect()
            self.assertListEqual(res.astuplelist(), list(range(1, 11)))

    def test_cache_map2(self):
        context = JitqContext()
        res = context.collection(range(0, 10)).map(lambda t: t + 2).collect()
        self.assertListEqual(res.astuplelist(), list(range(2, 12)))

    def test_cache_filter1(self):
        for _ in range(2):
            context = JitqContext()
            res = context.collection(range(
                0, 10)).filter(lambda t: t % 2 == 0).collect()
            self.assertListEqual(res.astuplelist(), list(range(0, 10, 2)))

    def test_cache_filter2(self):
        context = JitqContext()
        res = context.collection(range(
            0, 10)).filter(lambda t: t % 2 == 1).collect()
        self.assertListEqual(res.astuplelist(), list(range(1, 10, 2)))

        # TODO(sabir): test cases for the caching mechanism of all operators

    #
    # Integration
    #

    def test_map_filter_map(self):
        jitq_context = JitqContext()
        input_ = range(0, 10)

        def map_1(in_):
            return in_, in_ * 3

        def filter_1(in_):
            return in_[0] % 2 == 0

        def map_2(in_):
            return in_[0]

        data = jitq_context.collection(input_).map(map_1).filter(filter_1).map(
            map_2)
        res = data.collect()
        truth = list(map(map_2, filter(filter_1, map(map_1, input_))))
        self.assertListEqual(res.astuplelist(), truth)

    def test_all_operators(self):
        context = JitqContext()
        input_1 = context.range_(0, 10).map(lambda i: (i, 1))
        input_2 = context.collection(range(0, 10)).map(lambda i: (2 * i, i))
        input3 = context.collection(range(0, 10)).map(lambda i: (1, 1))
        step1 = input_1.join(
            input_2).map(lambda t: (t[0], t[1][1])).reduce_by_key(
            lambda a, b: a + b).cartesian(input3)
        step2 = step1.filter(lambda t, s: t[0] % 3 == 0).map(
            lambda s, t: t[0]).reduce(
            lambda tuple_1, tuple_2: tuple_1 + tuple_2)
        self.assertEqual(step2, 20)
