from blaze.blaze_context import BlazeContext


def test_join():
    bc = BlazeContext()
    data1 = bc.collection(range(0, 100)).map(lambda v: (v, 1))
    data2 = bc.collection(range(50, 150)).map(lambda v: (v, 1))

    joined = data1.join(data2).map(lambda v: v[0])
    res = joined.collect()
    print(res)
    assert res == range(50, 100)

test_join()