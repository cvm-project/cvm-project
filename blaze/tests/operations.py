import unittest

from blaze.blaze_context import BlazeContext


class TestJoin(unittest.TestCase):
    def test_overlap(self):
        bc = BlazeContext()
        data1 = bc.collection(range(0, 100)).map(lambda v: (v, 1))
        data2 = bc.collection(range(50, 150)).map(lambda v: (v, 1))

        joined = data1.join(data2).map(lambda v: v[0])
        res = joined.collect()
        self.assertEqual(res, list(range(50, 100)))


class TestOps(unittest.TestCase):
    def test_filter(self):
        bc = BlazeContext()
        d = bc.collection(range(0, 10)).filter(lambda w: w % 2 == 0)
        res = d.collect()
        self.assertEqual(res, list(filter(lambda w: w % 2 == 0, range(0, 10))))


class TestFlatMap(unittest.TestCase):
    def test_flat_map(self):
        bc = BlazeContext()
        d = bc.collection(range(0, 10)).flat_map(lambda w: [0])
        res = d.collect()
        self.assertEqual(res, [0 for i in range(0, 10)])


class TestIntegration(unittest.TestCase):
    def test_map_filter_flat_map_filter_flat_map(self):
        bc = BlazeContext()
        d = bc.collection(range(0, 10)).map(lambda w: w * 3).filter(lambda w: w % 2 == 0).flat_map(
            lambda w: [0]).filter(lambda w: w % 3).flat_map(lambda w: [0])
        d.collect()


if __name__ == '__main__':
    unittest.main()
