#!/usr/bin/env python3

import random
import unittest
import argparse
import sys
from functools import reduce
from itertools import groupby
from operator import itemgetter

import numpy as np

from blaze.blaze_context import BlazeContext


class TestCollection(unittest.TestCase):
    def test_index(self):
        bc = BlazeContext()
        res = bc.collection([i ** 2 for i in range(10)], add_index=True).collect()
        res = [tuple(r) for r in res]
        self.assertListEqual(res, [(i, i ** 2) for i in range(10)])

    def test_tuple(self):
        bc = BlazeContext()
        collection = [(i, 2*i) for i in range(10)]
        res = bc.collection(collection).collect()
        res = [tuple(r) for r in res]
        self.assertListEqual(res, collection)

    def test_array(self):
        bc = BlazeContext()
        collection = [(i, 2*i) for i in range(10)]
        res = bc.collection(np.array(collection)).collect()
        res = [tuple(r) for r in res]
        self.assertListEqual(res, collection)

    def test_array_index(self):
        bc = BlazeContext()
        collection = [(i, 2*i) for i in range(10)]
        res = bc.collection(np.array(collection), add_index=True).collect()
        res = [tuple(r) for r in res]
        ref = [(i,) + r for i, r in enumerate(collection)]
        self.assertListEqual(res, ref)

class TestRange(unittest.TestCase):
    def test_range_simple(self):
        bc = BlazeContext()
        res = bc.range_(0, 10).collect()
        self.assertListEqual(list(res), list(range(10)))

    def test_range_start(self):
        bc = BlazeContext()
        res = bc.range_(10, 20).collect()
        self.assertListEqual(list(res), list(range(10, 20)))

    def test_range_step(self):
        bc = BlazeContext()
        res = bc.range_(10, 20, 2).collect()
        self.assertListEqual(list(res), list(range(10, 20, 2)))

    def test_range_float(self):
        bc = BlazeContext()
        res = bc.range_(10.5, 20.0, 2.0).collect()
        self.assertListEqual(list(res), list(np.arange(10.5, 20.0, 2.0)))

class TestJoin(unittest.TestCase):
    def test_overlap(self):
        bc = BlazeContext()
        input1 = [(r, r * 10) for r in range(10)]
        input2 = [(r, r * 13, r + 100) for r in range(5, 15)]
        data1 = bc.collection(input1)
        data2 = bc.collection(input2)

        joined = data1.join(data2)
        res = joined.collect()
        truth = [(r, r * 10, r * 13, r + 100) for r in range(5, 10)]
        self.assertListEqual([tuple(r) for r in res], truth)

    def test_repeated_keys(self):
        bc = BlazeContext()
        input1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = bc.collection(input1)
        data2 = bc.collection(input2)

        joined = data1.join(data2)
        res = joined.collect()
        truth = [(1, 2, 22, 33), (1, 3, 22, 33), (1, 2, 44, 55), (1, 3, 44, 55), (2, 4, 33, 44), (2, 6, 33, 44)]
        self.assertListEqual([tuple(r) for r in res], truth)

        # def test_count(self):
        #     bc = BlazeContext()
        #     input1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        #     input2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        #     data1 = bc.collection(input1)
        #     data2 = bc.collection(input2)
        #
        #     joined = data1.join(data2)
        #     res = joined.count()
        #     truth = 6
        #     self.assertEqual(res, truth)


class TestFilter(unittest.TestCase):
    def test_filter_count(self):
        bc = BlazeContext()
        d = bc.collection(range(0, 10)).filter(lambda w: w % 2 == 0)
        res = d.count()
        self.assertEqual(res, len(list(filter(lambda w: w % 2 == 0, range(0, 10)))))

    def test_filter_collect(self):
        bc = BlazeContext()
        d = bc.collection(range(0, 10)).filter(lambda w: w % 2 == 0)
        res = d.collect()
        self.assertListEqual(list(res), list(filter(lambda w: w % 2 == 0, range(0, 10))))


class TestMap(unittest.TestCase):
    def test_map_collect(self):
        bc = BlazeContext
        d = bc.collection(range(0, 10)).map(lambda t: (t, (t, t * 10)))
        res = d.collect()
        self.assertListEqual([(t[0], t[1], t[2]) for t in res], [(t, t, t * 10) for t in range(0, 10)])

    def test_map_count(self):
        bc = BlazeContext
        d = bc.collection(range(0, 10)).map(lambda t: (t, t * 10))
        res = d.count()
        self.assertEqual(res, 10)


# class TestFlatMap(unittest.TestCase):
#     def test_flat_map(self):
#         bc = BlazeContext()
#         d = bc.collection(range(0, 10)).flat_map(lambda w: [0])
#         res = d.collect()
#         self.assertEqual(res, [0 for i in range(0, 10)])


class TestReduce(unittest.TestCase):
    def test_reduce_sum(self):
        bc = BlazeContext()
        input = range(0, 10)
        d = bc.collection(input).reduce(lambda t1, t2: t1 + t2)
        self.assertEqual(d, sum(input))

    @unittest.skip("Feature not fully implemented")
    def test_reduce_tuple(self):
        bc = BlazeContext()
        input = [(r, r * 10) for r in range(0, 10)]
        res = bc.collection(input).reduce(lambda t1, t2: (t1[0] + t2[0], t1[1] + t2[1]))
        self.assertTupleEqual(res, (sum(map(lambda t: t[0], input)), sum(map(lambda t: t[1], input))))


class TestReduceByKey(unittest.TestCase):
    def test_reduce_by_key(self):
        bc = BlazeContext()
        input_ = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]
        d = bc.collection(input_).reduce_by_key(lambda t1, t2: t1 + t2).collect()
        truth = {(0, 2), (1, 3)}
        self.assertSetEqual(set(tuple(t) for t in d), truth)

    @unittest.skip("Feature not fully implemented")
    def test_reduce_by_key_tuple(self):
        bc = BlazeContext()
        input_ = [(0, 1, 2), (1, 1, 2), (1, 1, 2), (0, 1, 2), (1, 1, 2)]
        d = bc.collection(input_).reduce_by_key(lambda t1, t2: (t1[0] + t2[0], t1[1] + t2[1])).collect()
        truth = {(0, 2, 4), (1, 3, 6)}
        self.assertSetEqual(set(tuple(t) for t in d), truth)

    def test_grouped(self):
        bc = BlazeContext()
        input_ = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]
        reduce_func = lambda t1, t2: (t1[0] + t2[0], t1[1] + t2[1], t1[2] + t2[2], t1[3] + t2[3], t1[4] + t2[4])

        enumerated_input = [(i,) + t for i, t in enumerate(input_)]
        cart = [t1 + t2 for t1 in enumerated_input for t2 in enumerated_input]
        truth = [reduce(lambda t1, t2: (t1[0],) + reduce_func(t1[1:], t2[1:]), group)
                       for _, group in groupby(sorted(cart), key=itemgetter(0))]

        d = bc.collection(input_, add_index=True).cartesian(bc.collection(input_, add_index=True)) \
                .flatten().reduce_by_key(reduce_func).collect()

        self.assertEqual(sorted((tuple(t) for t in d)), list(sorted(truth)))


class TestCartesian(unittest.TestCase):
    def test_count(self):
        bc = BlazeContext()
        input1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = bc.collection(input1)
        data2 = bc.collection(input2)

        joined = data1.cartesian(data2)
        res = joined.count()
        truth = 20
        self.assertEqual(res, truth)

    def test_collect(self):
        bc = BlazeContext()
        input1 = [(1, 2), (1, 3)]
        input2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77)]
        data1 = bc.collection(input1)
        data2 = bc.collection(input2)

        joined = data1.cartesian(data2)
        res = joined.collect()
        truth = {(1, 2, 1, 22, 33), (1, 2, 1, 44, 55), (1, 2, 8, 66, 77), (1, 3, 1, 22, 33), (1, 3, 1, 44, 55),
                 (1, 3, 8, 66, 77)}
        self.assertSetEqual(set(tuple(t) for t in res), truth)


class TestCache(unittest.TestCase):
    def test_map1(self):
        for i in range(2):
            bc = BlazeContext
            res = bc.collection(range(0, 10)).map(lambda t: t + 1).collect()
            self.assertListEqual(list(res), list(range(1, 11)))

    def test_map2(self):
        bc = BlazeContext
        res = bc.collection(range(0, 10)).map(lambda t: t + 2).collect()
        self.assertListEqual(list(res), list(range(2, 12)))

    def test_filter1(self):
        for i in range(2):
            bc = BlazeContext
            res = bc.collection(range(0, 10)).filter(lambda t: t % 2 == 0).collect()
            self.assertListEqual(list(res), list(range(0, 10, 2)))

    def test_filter2(self):
        bc = BlazeContext
        res = bc.collection(range(0, 10)).filter(lambda t: t % 2 == 1).collect()
        self.assertListEqual(list(res), list(range(1, 10, 2)))

    # TODO(sabir): there should be test cases for the caching mechanism of all operators


class TestIntegration(unittest.TestCase):
    def test_map_filter_map(self):
        bc = BlazeContext()
        input_ = range(0, 10)
        mapF1 = lambda w: (w, w * 3)
        filtF1 = lambda w: w[0] % 2 == 0
        mapF2 = lambda w: w[0]
        d = bc.collection(input_).map(mapF1).filter(filtF1).map(mapF2)
        res = d.collect()
        self.assertListEqual(list(res), list(map(mapF2, filter(filtF1, map(mapF1, input_)))))

    def test_all_operators(self):
        bc = BlazeContext()
        input1 = bc.range_(0, 10).map(lambda i: (i,1))
        input2 = bc.collection(range(0, 10)).map(lambda i: (2*i,i))
        input3 = bc.collection(range(0, 10)).map(lambda i: (1,1))
        step1 = input1.join(input2).map(lambda t: (t[0], t[1][1])).reduce_by_key(lambda a, b: a + b).cartesian(input3)
        step2 = step1.filter(lambda t, s: t[0] % 3 == 0).map(lambda s, t: t[0]).reduce(lambda t1, t2: t1 + t2)
        self.assertEqual(step2, 20)

    @unittest.skip("Test case not fully implemented")
    def test_map_filter_join(self):
        bc = BlazeContext()
        input_ = [(1, 2), (5, 4)]
        data2 = bc.collection([(1, 33), (0, 0)]).map(lambda t: (t[0], t[0] * 3)).filter(lambda t: t[0] > -1)
        print(bc.collection(input_).map(lambda t: (t[0], t[1] * 10)).filter(lambda t: t[0] == 1).join(data2).collect())


if __name__ == '__main__':
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("-r", "--repeat", type=int, default=1,
                        help="Repeat tests given number of times")
    (args, unitargs) = parser.parse_known_args()
    unitargs.insert(0, "placeholder") # unittest ignores first arg
    for iteration in range(args.repeat):
        wasSuccessful = unittest.main(exit=False, argv=unitargs).result.wasSuccessful()
    if not wasSuccessful:
        sys.exit(1)
