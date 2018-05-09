#!/usr/bin/env python3

import unittest
from itertools import groupby
from functools import reduce
from operator import itemgetter
from numpy.testing import assert_array_equal
import numpy as np
import pandas as pd

from jitq.jitq_context import JitqContext


# Disable "too-many-public-methods": this is not an interface, but a collection
# of test cases, which may be long.
# pylint: disable=R0904
class TestCollection(unittest.TestCase):

    def test_scalar(self):
        context = JitqContext()
        input_ = range(10)
        res = context.collection(input_).map(lambda x: x + 0).collect()
        self.assertListEqual(res.astuplelist(), list(input_))

    def test_tuple(self):
        context = JitqContext()
        input_ = [(i, 2 * i) for i in range(10)]
        res = context.collection(input_).collect()
        self.assertListEqual(res.astuplelist(), input_)

    def test_scalar_index(self):
        context = JitqContext()
        input_ = range(10)
        res = context.collection(input_, add_index=True).collect()
        self.assertListEqual(res.astuplelist(), list(enumerate(input_)))

    def test_tuple_index(self):
        context = JitqContext()
        input_ = [(i, 2 * i) for i in range(10)]
        res = context.collection(input_, add_index=True).collect()
        truth = [(i, ) + r for i, r in enumerate(input_)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_array_scalar(self):
        context = JitqContext()
        input_ = range(10)
        res = context.collection(np.array(input_)) \
            .map(lambda x: x + 0).collect()
        self.assertListEqual(res.astuplelist(), list(input_))

    def test_array_tuple(self):
        context = JitqContext()
        input_ = [(i, 2 * i) for i in range(10)]
        # make sure to get array of records
        res = context.collection(np.array(input_, dtype="i4,i4")).collect()
        self.assertListEqual(res.astuplelist(), input_)

    def test_array_scalar_index(self):
        context = JitqContext()
        input_ = range(10)
        res = context.collection(np.array(input_), add_index=True) \
            .collect()
        self.assertListEqual(res.astuplelist(), list(enumerate(input_)))

    def test_array_tuple_index(self):
        context = JitqContext()
        input_ = [(i, 2 * i) for i in range(10)]
        res = context.collection(np.array(input_, dtype="i4,i4"),
                                 add_index=True).collect()
        truth = [(i,) + r for i, r in enumerate(input_)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_pandas_scalar(self):
        context = JitqContext()
        input_ = range(10)
        res = context.collection(pd.DataFrame(list(input_))[0]) \
            .map(lambda x: x + 0).collect()
        self.assertListEqual(res.astuplelist(), list(input_))

    def test_pandas_tuple(self):
        context = JitqContext()
        input_ = [(i, 2 * i) for i in range(10)]
        res = context.collection(pd.DataFrame(list(input_))).collect()
        self.assertListEqual(res.astuplelist(), input_)

    def test_pandas_scalar_index(self): \
            # pragma pylint: disable=invalid-name
        context = JitqContext()
        input_ = list(range(10))
        res = context.collection(pd.DataFrame(input_), add_index=True) \
            .collect()
        truth = list(enumerate([(i,) for i in input_]))
        self.assertListEqual(res.astuplelist(), truth)

    def test_pandas_tuple_index(self): \
            # pragma pylint: disable=invalid-name
        context = JitqContext()
        input_ = [(i, 2 * i) for i in range(10)]
        res = context.collection(pd.DataFrame(input_), add_index=True) \
            .collect()
        truth = [(i, ) + r for i, r in enumerate(input_)]
        self.assertListEqual(res.astuplelist(), truth)

    @unittest.skip("Not implemented")
    def test_array_tuple_hetosize(self):
        context = JitqContext()
        input_ = [(i, 2 * i) for i in range(10)] + \
                 [(i, i + 1, i * 2) for i in range(5)]
        res = context.collection(np.array(input_)).collect()
        self.assertListEqual(res.astuplelist(), input_)

    # pylint: disable=no-self-use
    @unittest.skip("Not implemented")
    def test_array_record(self):
        context = JitqContext()
        input_ = np.array(
            [(1, 2 * 2)], dtype=[('first_int', 'i4'), ('second_float', 'f4')])
        res = context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @unittest.skip("Not implemented")
    def test_2darray(self):
        context = JitqContext()
        input_ = np.array([np.array(range(10)), np.array(range(10))])
        res = context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @unittest.skip("Not implemented")
    def test_2darray_record(self):
        context = JitqContext()
        input_ = np.array(
            np.array([np.array([(i, i * 2)], dtype=[
                ('int', 'i4'), ('float', 'f4')]) for i in range(5)]))
        res = context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @unittest.skip("Not implemented")
    def test_array_record_array(self):
        context = JitqContext()
        input_ = np.array([(1, [[1, 2, 3], [4, 5, 6]])], dtype=[
                          ('first_int', 'i4'), ('second_array', '(2,3)f4')])
        res = context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    # pylint: disable=no-self-use
    @unittest.skip("Not implemented")
    def test_array_record_tuple(self):
        context = JitqContext()
        input_ = np.array([(1, (2, 3))], dtype=[
                          ('first_int', 'i4'), ('second_record', 'f4,f4')])
        res = context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @unittest.skip("Not implemented")
    def test_array_nested_record(self):
        context = JitqContext()
        engine_dt = np.dtype([('volume', float), ('cylinders', int)])
        # nest the dtypes
        car_dt = np.dtype([('color', int, 3), ('engine', engine_dt)])

        cars = np.array([
            ([255, 0, 0], (1.5, 8)),
            ([255, 0, 255], (5, 24)),
        ], dtype=car_dt)
        res = context.collection(np.array(cars)).collect()
        assert_array_equal(res, cars)


class TestRange(unittest.TestCase):
    def test_simple(self):
        context = JitqContext()
        res = context.range_(0, 10).collect()
        self.assertListEqual(res.astuplelist(), list(range(10)))

    def test_start(self):
        context = JitqContext()
        res = context.range_(10, 20).collect()
        self.assertListEqual(res.astuplelist(), list(range(10, 20)))

    def test_step(self):
        context = JitqContext()
        res = context.range_(10, 20, 2).collect()
        self.assertListEqual(res.astuplelist(), list(range(10, 20, 2)))

    def test_float(self):
        context = JitqContext()
        res = context.range_(10.5, 20.0, 2.0).collect()
        self.assertListEqual(res.astuplelist(),
                             list(np.arange(10.5, 20.0, 2.0)))

    def test_reiterable(self):
        context = JitqContext()
        res = context.range_(0, 10) \
                     .cartesian(context.range_(0, 10)) \
                     .count()
        self.assertEqual(res, 100)


class TestJoin(unittest.TestCase):

    def test_scalars(self):
        input_1 = range(10)
        input_2 = range(10)

        jitq_context = JitqContext()
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)
        res = data1.join(data2).collect()
        truth = [i for i in input_1 if i in input_2]
        self.assertListEqual(res.astuplelist(), truth)

    @unittest.skip("Bug in CPP back-end related to empty structs")
    def test_scalar_left(self):
        input_1 = range(10)
        input_2 = list(enumerate(range(10)))

        jitq_context = JitqContext()
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)
        res = data1.join(data2).collect()
        truth = [t for t in input_2 if t[0] in input_1]
        self.assertListEqual(res.astuplelist(), truth)

    def test_scalar_right(self):
        input_1 = list(enumerate(range(10)))
        input_2 = range(10)

        jitq_context = JitqContext()
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)
        res = data1.join(data2).collect()
        truth = [t for t in input_1 if t[0] in input_2]
        self.assertListEqual(res.astuplelist(), truth)

    def test_nonmatching_keys(self):
        input_1 = list(enumerate(range(10)))
        input_2 = [(float(a), b) for a, b in input_1]

        jitq_context = JitqContext()
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)
        self.assertRaises(TypeError, data1.join(data2).collect)

    def test_overlap(self):
        jitq_context = JitqContext()
        input_1 = [(r, r * 10) for r in range(10)]
        input_2 = [(r, r * 13, r + 100) for r in range(5, 15)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.join(data2).collect()
        truth = [(r, r * 10, r * 13, r + 100) for r in range(5, 10)]
        self.assertListEqual(res.astuplelist(), truth)

    @unittest.skip("Bug in CPP back-end related to empty structs")
    def test_left_single_field(self):
        jitq_context = JitqContext()
        input_1 = [(r,) for r in range(10)]
        input_2 = [(r, r * 13, r + 100) for r in range(5, 15)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.join(data2).collect()
        truth = [(r, r * 13, r + 100) for r in range(5, 10)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_right_single_field(self):
        jitq_context = JitqContext()
        input_1 = [(r, r * 10) for r in range(10)]
        input_2 = [(r,) for r in range(5, 15)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.join(data2).collect()
        truth = [(r, r * 10) for r in range(5, 10)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_repeated_keys(self):
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


class TestFilter(unittest.TestCase):

    def test_count_scalar(self):
        jitq_context = JitqContext()
        res = jitq_context.collection(range(0, 10)) \
            .filter(lambda w: w % 2 == 0) \
            .count()
        truth = len(list(filter(lambda w: w % 2 == 0, range(0, 10))))
        self.assertEqual(res, truth)

    def test_collect_scalar(self):
        jitq_context = JitqContext()
        res = jitq_context.collection(range(0, 10)) \
            .filter(lambda w: w % 2 == 0) \
            .collect()
        truth = list(filter(lambda w: w % 2 == 0, range(0, 10)))
        self.assertListEqual(res.astuplelist(), truth)

    def test_count_tuple(self):
        jitq_context = JitqContext()
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .filter(lambda w: w[0] % 2 == 0) \
            .count()
        truth = len(list(filter(lambda w: w[0] % 2 == 0, input_)))
        self.assertEqual(res, truth)

    def test_collect_tuple(self):
        jitq_context = JitqContext()
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .filter(lambda w: w[0] % 2 == 0) \
            .collect()
        truth = list(filter(lambda w: w[0] % 2 == 0, input_))
        self.assertListEqual(res.astuplelist(), truth)

    def test_constant_false(self):
        jitq_context = JitqContext()
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .filter(lambda t: False) \
            .collect()
        self.assertListEqual(res.astuplelist(), [])

    def test_move_around(self):
        jitq_context = JitqContext()
        input_ = list(range(10))
        res = jitq_context.collection(input_) \
            .map(lambda t: (0, t)) \
            .filter(lambda t: t[1] % 2 == 0) \
            .collect()
        truth = [(0, 0), (0, 2), (0, 4), (0, 6), (0, 8)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_multiple_writers1(self):
        jitq_context = JitqContext()
        input_ = list(range(10))
        res = jitq_context.collection(input_) \
            .map(lambda t: (t, t + 1)) \
            .filter(lambda t: (t[0] + t[1]) % 3 == 0) \
            .collect()
        truth = [(1, 2), (4, 5), (7, 8)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_multiple_writers2(self):
        jitq_context = JitqContext()
        input_ = list(range(10))
        res = jitq_context.collection(input_) \
            .map(lambda t: (t, t)) \
            .filter(lambda t: t[0] + t[1] < 10) \
            .collect()
        truth = list(enumerate(range(5)))
        self.assertListEqual(res.astuplelist(), truth)


class TestMap(unittest.TestCase):

    def test_scalar_to_scalar(self):
        jitq_context = JitqContext()
        res = jitq_context.collection(range(0, 10)) \
            .map(lambda i: i + 2) \
            .collect()
        self.assertListEqual(res.astuplelist(), [i + 2 for i in range(0, 10)])

    def test_scalar_to_tuple(self):
        jitq_context = JitqContext()
        res = jitq_context.collection(range(0, 10)) \
            .map(lambda i: (i, (i, i * 10))) \
            .collect()
        truth = [(i, (i, i * 10)) for i in range(0, 10)]
        self.assertListEqual(res.astuplelist(), truth)

    def test_tuple_to_scalar(self):
        jitq_context = JitqContext()
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .map(lambda t: t[0] + t[1]) \
            .collect()
        self.assertListEqual(res.astuplelist(), [t[0] + t[1] for t in input_])

    def test_tuple_to_tuple(self):
        jitq_context = JitqContext()
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .map(lambda t: (t[0] * 2, t[1] * 2)) \
            .collect()
        truth = [(t[0] * 2, t[1] * 2) for t in input_]
        self.assertListEqual(res.astuplelist(), truth)

    def test_count(self):
        jitq_context = JitqContext()
        res = jitq_context.collection(range(0, 10)) \
            .map(lambda t: (t, t * 10)) \
            .count()
        self.assertEqual(res, 10)


class TestReduce(unittest.TestCase):

    def test_sum(self):
        jitq_context = JitqContext()
        input_ = range(0, 10)
        data = jitq_context.collection(input_) \
            .reduce(lambda tuple_1, tuple_2: tuple_1 + tuple_2)
        self.assertEqual(data, sum(input_))

    def test_tuple(self):
        jitq_context = JitqContext()
        input_ = [(r, r * 10) for r in range(0, 10)]
        res = jitq_context.collection(input_) \
            .reduce(lambda tuple_1, tuple_2:
                    (tuple_1[0] + tuple_2[0], tuple_1[1] + tuple_2[1]))
        res = tuple(res)
        truth = (sum(map(lambda t: t[0], input_)),
                 sum(map(lambda t: t[1], input_)))
        self.assertTupleEqual(res, truth)

    @unittest.skip("Empty inputs to reduce currently not supported.")
    def test_count_empty(self):
        jitq_context = JitqContext()
        res = jitq_context.range_(0, 0).count()
        self.assertEqual(res, 0)


class TestReduceByKey(unittest.TestCase):

    def test_basic(self):
        jitq_context = JitqContext()
        input_ = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]
        data = jitq_context.collection(input_) \
            .reduce_by_key(lambda tuple_1, tuple_2: tuple_1 + tuple_2) \
            .collect()
        truth = {(0, 2), (1, 3)}
        self.assertSetEqual(set(data.astuples()), truth)

    def test_tuple(self):
        jitq_context = JitqContext()
        input_ = [(0, 1, 2), (1, 1, 2), (1, 1, 2), (0, 1, 2), (1, 1, 2)]
        data = jitq_context.collection(input_) \
            .reduce_by_key(lambda tuple_1, tuple_2:
                           (tuple_1[0] + tuple_2[0],
                            tuple_1[1] + tuple_2[1])) \
            .collect()
        truth = [(0, 2, 4), (1, 3, 6)]
        self.assertEqual(sorted(data.astuples()), sorted(truth))

    def test_grouped(self):
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
        grouped = groupby(sorted(cart), key=itemgetter(0))
        truth = [reduce(lambda tuple_1, tuple_2:
                        (tuple_1[0],) +
                        reduce_func(tuple_1[1:], tuple_2[1:]), group)
                 for _, group in grouped]
        data = jitq_context.collection(input_, add_index=True) \
            .cartesian(jitq_context.collection(input_, add_index=True)) \
            .reduce_by_key(reduce_func) \
            .collect()

        self.assertEqual(sorted(data.astuples()), sorted(truth))


class TestCartesian(unittest.TestCase):

    def test_count(self):
        jitq_context = JitqContext()
        input_1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        joined = data1.cartesian(data2)
        res = joined.count()
        truth = 20
        self.assertEqual(res, truth)

    def test_tuple_tuple(self):
        jitq_context = JitqContext()
        input_1 = [(1, 2), (1, 3)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.cartesian(data2).collect()
        truth = {x + y for x in input_1 for y in input_2}
        self.assertSetEqual(set(res.astuples()), truth)

    def test_scalar_tuple(self):
        jitq_context = JitqContext()
        input_1 = list(range(5))
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.cartesian(data2).collect()
        truth = [(x,) + y for x in input_1 for y in input_2]
        self.assertListEqual(sorted(res.astuples()), sorted(truth))

    def test_tuple_scalar(self):
        jitq_context = JitqContext()
        input_1 = [(1, 2), (1, 3)]
        input_2 = list(range(5))
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.cartesian(data2).collect()
        truth = [x + (y,) for x in input_1 for y in input_2]
        self.assertListEqual(sorted(res.astuples()), sorted(truth))

    def test_scalar_scalar(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 2))
        input2 = jitq_context.collection(range(0, 2))
        res = input1.cartesian(input2).collect()
        truth = [(0, 0), (0, 1), (1, 0), (1, 1)]
        self.assertListEqual(sorted(res.astuples()), sorted(truth))


class TestCache(unittest.TestCase):

    def test_map1(self):
        for _ in range(2):
            context = JitqContext()
            res = context.collection(range(0, 10)) \
                .map(lambda t: t + 1) \
                .collect()
            self.assertListEqual(list(res), list(range(1, 11)))

    def test_map2(self):
        context = JitqContext()
        res = context.collection(range(0, 10)) \
            .map(lambda t: t + 2) \
            .collect()
        self.assertListEqual(list(res), list(range(2, 12)))

    def test_filter1(self):
        for _ in range(2):
            context = JitqContext()
            res = context.collection(range(0, 10)) \
                .filter(lambda t: t % 2 == 0) \
                .collect()
            self.assertListEqual(list(res), list(range(0, 10, 2)))

    def test_filter2(self):
        context = JitqContext()
        res = context.collection(range(0, 10)) \
            .filter(lambda t: t % 2 == 1) \
            .collect()
        self.assertListEqual(list(res), list(range(1, 10, 2)))

        # TODO(sabir): test cases for the caching mechanism of all operators


class TestSortedness(unittest.TestCase):
    def test_index_grouped(self):
        context = JitqContext()
        res = context.collection(list(range(10)), add_index=True) \
                     .reduce_by_key(lambda i1, i2: i1 + i2)\
                     .collect()
        truth = list(enumerate(range(10)))
        self.assertListEqual(sorted(res.astuplelist()), truth)

    def test_range_grouped(self):
        context = JitqContext()
        res = context.range_(0, 10) \
                     .map(lambda i: (i, i)) \
                     .reduce_by_key(lambda i1, i2: i1 + i2)\
                     .collect()
        truth = list(enumerate(range(10)))
        self.assertListEqual(sorted(res.astuplelist()), truth)

    def test_cartesian_grouped(self):
        context = JitqContext()
        res = context.collection(list(range(5))) \
                     .cartesian(context.range_(0, 2)) \
                     .map(lambda t: (t[1], t[0])) \
                     .reduce_by_key(lambda i1, i2: i1 + i2)\
                     .collect()
        truth = [(i, sum(range(5))) for i in range(2)]
        self.assertListEqual(sorted(res.astuplelist()), truth)

    def test_join_right_grouped(self):
        context = JitqContext()
        res = context.range_(0, 10) \
                     .map(lambda i: (int(i / 2), i)) \
                     .join(context.range_(0, 10)) \
                     .map(lambda t: (t[1], t[0])) \
                     .reduce_by_key(lambda i1, i2: i1 + i2)\
                     .collect()
        truth = [(i, int(i / 2)) for i in range(10)]
        self.assertListEqual(sorted(res.astuplelist()), truth)

    def test_join_key_grouped(self):
        context = JitqContext()
        res = context.range_(0, 10) \
                     .map(lambda i: (i, int(i / 2))) \
                     .join(context.range_(0, 10)) \
                     .reduce_by_key(lambda i1, i2: i1 + i2)\
                     .collect()
        truth = [(i, int(i / 2)) for i in range(10)]
        self.assertListEqual(sorted(res.astuplelist()), truth)

    def test_join_left_grouped(self):
        context = JitqContext()
        res = context.range_(0, 10) \
                     .map(lambda i: (i, int(i / 2))) \
                     .join(context.range_(0, 10)) \
                     .map(lambda t: (t[1], t[0])) \
                     .reduce_by_key(lambda i1, i2: i1 + i2)\
                     .collect()
        truth = [(i, i * 4 + 1) for i in range(5)]
        self.assertListEqual(sorted(res.astuplelist()), truth)


class TestFilterPushDown(unittest.TestCase):
    def test_push_down_bifurcation(self):
        context = JitqContext()
        input_ = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        res = context.collection(input_) \
                     .join(context.collection(input_)) \
                     .filter(lambda a: a[1] == a[2]) \
                     .collect()
        truth = [(c, a, a) for c, a in input_]
        self.assertListEqual(sorted(res.astuplelist()), sorted(truth))

    def test_push_up(self):
        context = JitqContext()
        input_ = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        res = context.collection(input_) \
                     .filter(lambda a: a[0] % 2 == 0) \
                     .join(context.collection(input_)) \
                     .collect()
        truth = [(2, 4, 4), (2, 4, 6), (2, 6, 4), (2, 6, 6)]
        self.assertListEqual(sorted(res.astuplelist()), sorted(truth))

    def test_dag1(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 2))
        result = input1.map(lambda t: t).filter(lambda i: i == 0) \
                       .cartesian(input1).collect()
        truth = [(0, 0), (0, 1)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_dag2(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 2))
        result = input1.cartesian(input1) \
                       .filter(lambda t: t[0] == 0) \
                       .collect()
        truth = [(0, 0), (0, 1)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_dag3(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 2))
        result = input1.cartesian(input1) \
                       .filter(lambda t: t[1] == 0) \
                       .collect()
        truth = [(0, 0), (1, 0)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_dag4(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input1.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[0] == 2) \
                       .collect()
        truth = [(2, 3, 1)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_dag5(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input1.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[1] == 2) \
                       .collect()
        truth = [(1, 2, 0)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_dag6(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input1.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[2] == 2) \
                       .collect()
        truth = [(3, 4, 2)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))


class TestIntegration(unittest.TestCase):

    def test_reexecute_dag(self):
        jitq_context = JitqContext()
        input_ = jitq_context.collection(range(0, 2))
        res1 = input_.collect()
        res2 = input_.collect()
        self.assertListEqual(res1.astuplelist(), res2.astuplelist())

    def test_reuse_and_modify_dag(self):
        jitq_context = JitqContext()
        input_ = jitq_context.collection(range(0, 2))
        res1 = input_.collect()
        res2 = input_.filter(lambda t: True).collect()
        self.assertListEqual(res1.astuplelist(), res2.astuplelist())

    def test_dag_explosion(self):
        jitq_context = JitqContext()
        input_ = range(0, 10)
        operator = jitq_context.collection(input_)
        for _ in range(10):
            operator = operator.join(operator)
        res = operator.collect()
        self.assertListEqual(res.astuplelist(), list(input_))

    def test_dag1(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 2))
        result = input1.cartesian(input1).collect()
        truth = [(0, 0), (0, 1), (1, 0), (1, 1)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_dag2(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 2)).filter(lambda t: True)
        result = input1.cartesian(input1).collect()
        truth = [(0, 0), (0, 1), (1, 0), (1, 1)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_map_filter_map(self):
        jitq_context = JitqContext()
        input_ = range(0, 10)

        def map_1(in_):
            return in_, in_ * 3

        def filter_1(in_):
            return in_[0] % 2 == 0

        def map_2(in_):
            return in_[0]

        res = jitq_context.collection(input_) \
            .map(map_1) \
            .filter(filter_1) \
            .map(map_2) \
            .collect()
        truth = list(map(map_2, filter(filter_1, map(map_1, input_))))
        self.assertListEqual(list(res), truth)

    def test_join_input_order(self):
        jitq_context = JitqContext()
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        input2 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input2.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[1] == 2) \
                       .collect()
        truth = [(1, 2, 0)]
        self.assertListEqual(sorted(result.astuplelist()), sorted(truth))

    def test_all_operators(self):
        context = JitqContext()
        input_1 = context.range_(0, 10).map(lambda i: (i, 1))
        input_2 = context.collection(range(0, 10)).map(lambda i: (2 * i, i))
        input_3 = context.collection(range(0, 10)).map(lambda i: (1, 1))
        res = input_1.join(input_2) \
            .map(lambda t: (t[0], t[2])) \
            .reduce_by_key(lambda a, b: a + b) \
            .cartesian(input_3) \
            .filter(lambda t: t[0] % 3 == 0) \
            .map(lambda t: t[2]) \
            .reduce(lambda tuple_1, tuple_2: tuple_1 + tuple_2)
        self.assertEqual(res, 20)
