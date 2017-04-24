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


if __name__ == '__main__':
    unittest.main()
