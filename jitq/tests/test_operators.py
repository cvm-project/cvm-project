#!/usr/bin/env python3

import unittest
import argparse
import sys

import pandas
from functools import reduce
from itertools import groupby
from operator import itemgetter

import numpy as np

from jitq.jitq_context import JitqContext


class TestCollection(unittest.TestCase):
    def test_index(self):
        context = JitqContext()
        res = context.collection(
            [i**2 for i in range(10)], add_index=True).collect()
        res = [tuple(r) for r in res]
        self.assertListEqual(res, [(i, i**2) for i in range(10)])

    def test_tuple(self):
        context = JitqContext()
        collection = [(i, 2 * i) for i in range(10)]
        res = context.collection(collection).collect()
        res = [tuple(r) for r in res]
        self.assertListEqual(res, collection)

    def test_array(self):
        context = JitqContext()
        collection = [(i, 2 * i) for i in range(10)]
        res = context.collection(np.array(collection)).collect()
        res = [tuple(r) for r in res]
        self.assertListEqual(res, collection)

    def test_array_index(self):
        context = JitqContext()
        collection = [(i, 2 * i) for i in range(10)]
        res = context.collection(
            np.array(collection), add_index=True).collect()
        res = [tuple(r) for r in res]
        ref = [(i, ) + r for i, r in enumerate(collection)]
        self.assertListEqual(res, ref)

    def test_pandas_dataframe(self):
        context = JitqContext()
        array = pandas.DataFrame([(i, 2 * i) for i in range(10)])
        res = context.collection(array).collect()
        res = [tuple(r) for r in res]
        truth = [tuple(r) for r in array.values.tolist()]
        self.assertListEqual(truth, res)


class TestRange(unittest.TestCase):
    def test_range_simple(self):
        context = JitqContext()
        res = context.range_(0, 10).collect()
        self.assertListEqual(list(res), list(range(10)))

    def test_range_start(self):
        context = JitqContext()
        res = context.range_(10, 20).collect()
        self.assertListEqual(list(res), list(range(10, 20)))

    def test_range_step(self):
        context = JitqContext()
        res = context.range_(10, 20, 2).collect()
        self.assertListEqual(list(res), list(range(10, 20, 2)))

    def test_range_float(self):
        context = JitqContext()
        res = context.range_(10.5, 20.0, 2.0).collect()
        self.assertListEqual(list(res), list(np.arange(10.5, 20.0, 2.0)))


class TestJoin(unittest.TestCase):
    def test_overlap(self):
        jitq_context = JitqContext()
        input1 = [(r, r * 10) for r in range(10)]
        input2 = [(r, r * 13, r + 100) for r in range(5, 15)]
        data1 = jitq_context.collection(input1)
        data2 = jitq_context.collection(input2)

        joined = data1.join(data2)
        res = joined.collect()
        truth = [(r, r * 10, r * 13, r + 100) for r in range(5, 10)]
        self.assertListEqual([tuple(r) for r in res], truth)

    def test_repeated_keys(self):
        jitq_context = JitqContext()
        input1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = jitq_context.collection(input1)
        data2 = jitq_context.collection(input2)

        joined = data1.join(data2)
        res = joined.collect()
        truth = [(1, 2, 22, 33), (1, 3, 22, 33), (1, 2, 44, 55),
                 (1, 3, 44, 55), (2, 4, 33, 44), (2, 6, 33, 44)]
        self.assertListEqual([tuple(r) for r in res], truth)


class TestFilter(unittest.TestCase):
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
        self.assertListEqual(
            list(res), list(filter(lambda w: w % 2 == 0, range(0, 10))))


class TestMap(unittest.TestCase):
    def test_map_collect(self):
        jitq_context = JitqContext
        data = jitq_context.collection(range(
            0, 10)).map(lambda t: (t, (t, t * 10)))
        res = data.collect()
        self.assertListEqual([(t[0], t[1], t[2]) for t in res],
                             [(t, t, t * 10) for t in range(0, 10)])

    def test_map_count(self):
        jitq_context = JitqContext
        data = jitq_context.collection(range(0, 10)).map(lambda t: (t, t * 10))
        res = data.count()
        self.assertEqual(res, 10)


class TestReduce(unittest.TestCase):
    def test_reduce_sum(self):
        jitq_context = JitqContext()
        inpt = range(0, 10)
        data = jitq_context.collection(inpt).reduce(lambda t1, t2: t1 + t2)
        self.assertEqual(data, sum(inpt))

    def test_reduce_tuple(self):
        jitq_context = JitqContext()
        inpt = [(r, r * 10) for r in range(0, 10)]
        res = jitq_context.collection(inpt).reduce(
            lambda t1, t2: (t1[0] + t2[0], t1[1] + t2[1]))
        res = tuple(res)
        truth = (sum(map(lambda t: t[0], inpt)), sum(
            map(lambda t: t[1], inpt)))
        self.assertTupleEqual(res, truth)


class TestReduceByKey(unittest.TestCase):
    def test_reduce_by_key(self):
        jitq_context = JitqContext()
        inpt = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]
        data = jitq_context.collection(inpt).reduce_by_key(
            lambda t1, t2: t1 + t2).collect()
        truth = {(0, 2), (1, 3)}
        self.assertSetEqual(set(tuple(t) for t in data), truth)

    def test_reduce_by_key_tuple(self):
        jitq_context = JitqContext()
        input_ = [(0, 1, 2), (1, 1, 2), (1, 1, 2), (0, 1, 2), (1, 1, 2)]
        data = jitq_context.collection(input_).reduce_by_key(
            lambda t1, t2: (t1[0] + t2[0], t1[1] + t2[1])).collect()
        truth = [(0, 2, 4), (1, 3, 6)]
        self.assertEqual(sorted([tuple(t) for t in data]), sorted(truth))

    def test_grouped(self):
        jitq_context = JitqContext()
        input_ = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]
        reduce_func = lambda t1, t2: (t1[0] + t2[0], t1[1] + t2[1], t1[2] + t2[2], t1[3] + t2[3], t1[4] + t2[4])

        enumerated_input = [(i, ) + t for i, t in enumerate(input_)]
        cart = [t1 + t2 for t1 in enumerated_input for t2 in enumerated_input]
        truth = [
            reduce(lambda t1, t2: (t1[0], ) + reduce_func(t1[1:], t2[1:]),
                   group)
            for _, group in groupby(sorted(cart), key=itemgetter(0))
        ]

        data = jitq_context.collection(input_, add_index=True).cartesian(
            jitq_context.collection(input_, add_index=True)) \
            .flatten().reduce_by_key(reduce_func).collect()

        self.assertEqual(sorted((tuple(t) for t in data)), list(sorted(truth)))


class TestCartesian(unittest.TestCase):
    def test_count(self):
        jitq_context = JitqContext()
        input1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = jitq_context.collection(input1)
        data2 = jitq_context.collection(input2)

        joined = data1.cartesian(data2)
        res = joined.count()
        truth = 20
        self.assertEqual(res, truth)

    def test_collect(self):
        jitq_context = JitqContext()
        input1 = [(1, 2), (1, 3)]
        input2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77)]
        data1 = jitq_context.collection(input1)
        data2 = jitq_context.collection(input2)

        joined = data1.cartesian(data2)
        res = joined.collect()
        truth = {(1, 2, 1, 22, 33), (1, 2, 1, 44, 55), (1, 2, 8, 66, 77),
                 (1, 3, 1, 22, 33), (1, 3, 1, 44, 55), (1, 3, 8, 66, 77)}
        self.assertSetEqual(set(tuple(t) for t in res), truth)


class TestCache(unittest.TestCase):
    def test_map1(self):
        for _ in range(2):
            context = JitqContext
            res = context.collection(range(0,
                                           10)).map(lambda t: t + 1).collect()
            self.assertListEqual(list(res), list(range(1, 11)))

    def test_map2(self):
        context = JitqContext
        res = context.collection(range(0, 10)).map(lambda t: t + 2).collect()
        self.assertListEqual(list(res), list(range(2, 12)))

    def test_filter1(self):
        for _ in range(2):
            context = JitqContext
            res = context.collection(range(
                0, 10)).filter(lambda t: t % 2 == 0).collect()
            self.assertListEqual(list(res), list(range(0, 10, 2)))

    def test_filter2(self):
        context = JitqContext
        res = context.collection(range(
            0, 10)).filter(lambda t: t % 2 == 1).collect()
        self.assertListEqual(list(res), list(range(1, 10, 2)))

    # TODO(sabir): test cases for the caching mechanism of all operators


class TestIntegration(unittest.TestCase):
    def test_map_filter_map(self):
        jitq_context = JitqContext()
        input_ = range(0, 10)
        map_1 = lambda w: (w, w * 3)
        filter_1 = lambda w: w[0] % 2 == 0
        map_2 = lambda w: w[0]
        data = jitq_context.collection(input_).map(map_1).filter(filter_1).map(
            map_2)
        res = data.collect()
        self.assertListEqual(
            list(res), list(map(map_2, filter(filter_1, map(map_1, input_)))))

    def test_all_operators(self):
        context = JitqContext()
        input1 = context.range_(0, 10).map(lambda i: (i, 1))
        input2 = context.collection(range(0, 10)).map(lambda i: (2 * i, i))
        input3 = context.collection(range(0, 10)).map(lambda i: (1, 1))
        step1 = input1.join(
            input2).map(lambda t: (t[0], t[1][1])).reduce_by_key(
                lambda a, b: a + b).cartesian(input3)
        step2 = step1.filter(lambda t, s: t[0] % 3 == 0).map(
            lambda s, t: t[0]).reduce(lambda t1, t2: t1 + t2)
        self.assertEqual(step2, 20)


def parse_args():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument(
        "-r",
        "--repeat",
        type=int,
        default=1,
        help="Repeat tests given number of times")
    args, unit_args = parser.parse_known_args()
    unit_args.insert(0, "placeholder")  # unittest ignores first arg
    return args, unit_args


def run_tests(args, unit_args):
    for _ in range(args.repeat):
        was_successful = unittest.main(
            exit=False, argv=unit_args).result.wasSuccessful()
        if not was_successful:
            sys.exit(1)


if __name__ == '__main__':
    run_tests(*parse_args())
