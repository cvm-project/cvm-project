#!/usr/bin/env python3
# pylint: disable=too-many-lines
# pylint: disable=too-many-public-methods
# pylint: disable=redefined-outer-name  # required by pytest
# pylint: disable=no-self-use           # classes used for grouping tests

from functools import reduce
from itertools import groupby
from io import StringIO
from operator import itemgetter

import numpy as np
from numpy.testing import assert_array_equal
import numba
import pandas as pd
import pyarrow as pa
import pyarrow.parquet as pq
import pytest


class TestCollection:

    def test_scalar(self, jitq_context):
        input_ = range(10)
        res = jitq_context.collection(input_).map(lambda x: x + 0).collect()
        assert sorted(res.astuples()) == list(input_)

    def test_tuple(self, jitq_context):
        input_ = [(i, 2 * i) for i in range(10)]
        res = jitq_context.collection(input_).collect()
        assert sorted(res.astuples()) == input_

    def test_scalar_index(self, jitq_context):
        input_ = range(10)
        res = jitq_context.collection(input_, add_index=True).collect()
        assert sorted(res.astuples()) == list(enumerate(input_))

    def test_tuple_index(self, jitq_context):
        input_ = [(i, 2 * i) for i in range(10)]
        res = jitq_context.collection(input_, add_index=True).collect()
        truth = [(i, ) + r for i, r in enumerate(input_)]
        assert sorted(res.astuples()) == truth

    def test_array_scalar(self, jitq_context):
        input_ = range(10)
        res = jitq_context.collection(np.array(input_)) \
            .map(lambda x: x + 0).collect()
        assert sorted(res.astuples()) == list(input_)

    def test_array_tuple(self, jitq_context):
        input_ = [(i, 2 * i) for i in range(10)]
        input_array = np.array(input_, dtype="i4,i4")
        # make sure to get array of records
        res = jitq_context.collection(input_array).collect()
        assert sorted(res.astuples()) == input_

    def test_array_scalar_index(self, jitq_context):
        input_ = range(10)
        res = jitq_context.collection(np.array(input_), add_index=True) \
            .collect()
        assert sorted(res.astuples()) == list(enumerate(input_))

    def test_array_tuple_index(self, jitq_context):
        input_ = [(i, 2 * i) for i in range(10)]
        res = jitq_context.collection(np.array(input_, dtype="i4,i4"),
                                      add_index=True).collect()
        truth = [(i,) + r for i, r in enumerate(input_)]
        assert sorted(res.astuples()) == truth

    def test_array_mixed_widths(self, jitq_context):
        input_ = [(np.int32(i), np.int64(2 * i)) for i in range(10)]
        res = jitq_context.collection(input_) \
            .map(lambda t: (t[0] + 0, t[1] + 0)).collect()
        assert sorted(res.astuples()) == input_

    def test_array_aligned_tuple(self, jitq_context):
        dtype = np.dtype([('x', np.int32), ('y', np.int64)], align=True)
        input_ = [(np.int32(i), np.int64(2 * i)) for i in range(10)]
        input_array = np.array(input_, dtype=dtype)

        with pytest.raises(NotImplementedError):
            jitq_context.collection(input_array) \
                .map(lambda t: (t.x + 0, t.y + 0)).collect()

    def test_array_from_rdd(self, jitq_context):
        res1 = jitq_context.range_(0, 10).collect()
        col = jitq_context.collection(res1)
        res2 = col.collect()
        assert res1.astuplelist() == res2.astuplelist()

        # Find out data pointer of first result
        from json import loads
        res1_ptr = loads(res1.handle.string)[0]['fields'][0]['data']

        # Find out data pointer of `col`
        from cffi import FFI
        ffi = FFI()
        col_ptr = col.parents[0].data.__array_interface__['data'][0]
        col_ptr = int(ffi.cast("uintptr_t", ffi.cast("void*", col_ptr)))

        # Assert that we reuse the same memory (no copy involved)
        assert res1_ptr == col_ptr

    def test_pandas_scalar(self, jitq_context):
        input_ = range(10)
        res = jitq_context.collection(pd.DataFrame(list(input_))[0]) \
            .map(lambda x: x + 0).collect()
        assert sorted(res.astuples()) == list(input_)

    def test_pandas_tuple(self, jitq_context):
        input_ = [(i, 2.0 * i) for i in range(10)]
        input_df = pd.DataFrame(list(input_), columns=['v0', 'v1'])
        res = jitq_context.collection(input_df) \
            .map(lambda t: (t.v0, t.v1 + 0)).collect()
        assert sorted(res.astuples()) == input_

    def test_pandas_scalar_index(self, jitq_context): \
            # pragma pylint: disable=invalid-name
        input_ = list(range(10))
        res = jitq_context.collection(pd.DataFrame(pd.DataFrame(
            input_, columns=["f0"])), add_index=True) \
            .map(lambda x: (x.f0, x.f00 + 0)) \
            .collect()
        truth = list(enumerate([(i,) for i in input_]))
        assert sorted(res.astuples()) == truth

    def test_pandas_tuple_index(self, jitq_context): \
            # pragma pylint: disable=invalid-name
        input_ = [(i, 2 * i) for i in range(10)]
        input_df = pd.DataFrame(list(input_), columns=['v0', 'v1'])
        res = jitq_context.collection(input_df, add_index=True) \
            .map(lambda t: (t.v00, t.v0, t.v1 + 0)) \
            .collect()
        truth = [(i, ) + r for i, r in enumerate(input_)]
        assert sorted(res.astuples()) == truth

    @pytest.mark.skip("Not implemented")
    def test_array_tuple_hetosize(self, jitq_context):
        input_ = [(i, 2 * i) for i in range(10)] + \
                 [(i, i + 1, i * 2) for i in range(5)]
        res = jitq_context.collection(np.array(input_)).collect()
        assert sorted(res.astuples()) == input_

    @pytest.mark.skip("Not implemented")
    def test_array_record(self, jitq_context):
        input_ = np.array(
            [(1, 2 * 2)], dtype=[('first_int', 'i4'), ('second_float', 'f4')])
        res = jitq_context.collection(np.array(input_)) \
            .filter(lambda t: t.first_int == 1 or t.second_float == 4) \
            .collect()
        assert_array_equal(res, input_)

    @pytest.mark.skip("Not implemented")
    def test_2darray(self, jitq_context):
        input_ = np.array([np.array(range(10)), np.array(range(10))])
        res = jitq_context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @pytest.mark.skip("Not implemented")
    def test_2darray_record(self, jitq_context):
        input_ = np.array(
            np.array([np.array([(i, i * 2)], dtype=[
                ('int', 'i4'), ('float', 'f4')]) for i in range(5)]))
        res = jitq_context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @pytest.mark.skip("Not implemented")
    def test_array_record_array(self, jitq_context):
        input_ = np.array([(1, [[1, 2, 3], [4, 5, 6]])], dtype=[
                          ('first_int', 'i4'), ('second_array', '(2,3)f4')])
        res = jitq_context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @pytest.mark.skip("Not implemented")
    def test_array_record_tuple(self, jitq_context):
        input_ = np.array([(1, (2, 3))], dtype=[
                          ('first_int', 'i4'), ('second_record', 'f4,f4')])
        res = jitq_context.collection(np.array(input_)).collect()
        assert_array_equal(res, input_)

    @pytest.mark.skip("Not implemented")
    def test_array_nested_record(self, jitq_context):
        engine_dt = np.dtype([('volume', float), ('cylinders', int)])
        # nest the dtypes
        car_dt = np.dtype([('color', int, 3), ('engine', engine_dt)])

        cars = np.array([
            ([255, 0, 0], (1.5, 8)),
            ([255, 0, 255], (5, 24)),
        ], dtype=car_dt)
        res = jitq_context.collection(np.array(cars)).collect()
        assert_array_equal(res, cars)


class TestRange:

    def test_simple(self, jitq_context):
        res = jitq_context.range_(0, 10).collect()
        assert sorted(res.astuples()) == list(range(10))

    def test_start(self, jitq_context):
        res = jitq_context.range_(10, 20).collect()
        assert sorted(res.astuples()) == list(range(10, 20))

    def test_step(self, jitq_context):
        res = jitq_context.range_(10, 20, 2).collect()
        assert sorted(res.astuples()) == list(range(10, 20, 2))

    def test_float(self, jitq_context):
        res = jitq_context.range_(10.5, 20.0, 2.0).collect()
        assert sorted(res.astuples()) == list(np.arange(10.5, 20.0, 2.0))

    def test_reiterable(self, jitq_context):
        res = jitq_context.range_(0, 10) \
            .cartesian(jitq_context.range_(0, 10)) \
            .count()
        assert res == 100


class TestParquet:

    @pytest.fixture(autouse=True)
    def parquet_files(self, filesystem_instance):
        data = StringIO("""
            a;b;i32;i64;f;d
            1;1;1;1;1;1
            1;2;2;2;2;2
            1;3;3;3;3;3
            2;1;4;4;4;4
            2;2;5;5;5;5
            2;3;6;6;6;6
            """)

        df = pd.read_csv(data, sep=';',
                         index_col=False,
                         skipinitialspace=True,
                         dtype={'i32': 'int32',
                                'f': 'float32',
                                'd': 'float64'})
        table = pa.Table.from_pandas(df)

        for i in range(3):
            filename = 'test-{:05d}.parquet'.format(i)
            pq.write_table(table, filesystem_instance.to_local(filename),
                           row_group_size=2)
            filesystem_instance.copy_to_remote(filename)

        yield filesystem_instance

        for i in range(3):
            filename = 'test-{:05d}.parquet'.format(i)
            filesystem_instance.remove_from_remote(filename)

    def test_single_file(self, jitq_context, parquet_files):
        filename = 'test-00000.parquet'
        cols = [(0, numba.int64), (1, numba.int64)]

        res = jitq_context \
            .read_parquet(parquet_files.to_remote(filename), cols) \
            .collect()
        truth = [(1, 1), (1, 2), (1, 3), (2, 1), (2, 2), (2, 3)]
        assert list(res.astuples()) == truth

    def test_multiple_files(self, jitq_context, parquet_files):
        file_pattern = 'test-%1$05d.parquet'
        cols = [(0, numba.int64), (1, numba.int64)]

        res = jitq_context \
            .read_parquet(parquet_files.to_remote(file_pattern),
                          cols, (0, 3)) \
            .count()
        assert res == 18

    def test_empty_range(self, jitq_context, parquet_files):
        file_pattern = 'test-%1$05d.parquet'
        cols = [(0, numba.int64), (1, numba.int64)]

        res = jitq_context \
            .read_parquet(parquet_files.to_remote(file_pattern),
                          cols, (0, 0)) \
            .count()
        assert res == 0

    def test_filter(self, jitq_context, parquet_files):
        filename = 'test-00000.parquet'
        cols = [(0, numba.int64, [(1, 1)]),
                (4, numba.float32, [(3.0, 6.0)])]

        res = jitq_context \
            .read_parquet(parquet_files.to_remote(filename), cols) \
            .collect()
        truth = [(1, 3.0), (2, 4.0)]
        assert list(res.astuples()) == truth

    def test_types(self, jitq_context, parquet_files):
        file_pattern = 'test-%1$05d.parquet'
        cols = [
            (2, numba.int32),
            (3, numba.int64),
            (4, numba.float32),
            (5, numba.float64),
        ]

        res = jitq_context \
            .read_parquet(parquet_files.to_remote(file_pattern), cols) \
            .count()
        assert res == 6

    def test_write_file(self, jitq_context, parquet_files):
        filename = 'test-00000.parquet'
        data = list(enumerate(range(10)))
        column_names = ['a', 'b']

        # Note: the effect of this configuration is not tested automatically,
        # but can be verified with parquet-tools or similar.
        conf = {
            'writer_properties': {
                'compression': 'SNAPPY',
                'path_properties': {
                    'a': {
                        'compression': 'GZIP',
                        'enable_dictionary': False,
                        'encoding': 'PLAIN',
                    },
                },
            },
            "target_row_group_size": 3,
        }

        ret = jitq_context.collection(data) \
            .to_parquet(parquet_files.to_remote(filename), column_names, conf)
        assert ret == parquet_files.to_remote(filename)

        parquet_files.copy_from_remote(filename)
        table = pq.read_table(parquet_files.to_local(filename))
        assert column_names == [c.name for c in table.columns]
        res = zip(*[c.to_pylist() for c in table.columns])
        assert data == sorted(res)

    def test_write_empty_file(self, jitq_context, parquet_files):
        filename = 'test-00000.parquet'
        column_names = ['a', 'b']

        ret = jitq_context.range_(0, 0) \
            .map(lambda x: (x, x)) \
            .to_parquet(parquet_files.to_remote(filename), column_names)
        assert ret == parquet_files.to_remote(filename)

        parquet_files.copy_from_remote(filename)
        table = pq.read_table(parquet_files.to_local(filename))
        assert column_names == [c.name for c in table.columns]
        res = zip(*[c.to_pylist() for c in table.columns])
        assert [] == sorted(res)


class TestJoin:

    def test_scalars(self, jitq_context):
        input_1 = range(10)
        input_2 = range(10)

        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)
        res = data1.join(data2).collect()
        truth = [i for i in input_1 if i in input_2]
        assert sorted(res.astuples()) == truth

    def test_scalar_left(self, jitq_context):
        input_1 = range(10)
        input_2 = list(enumerate(range(10)))

        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)
        res = data1.join(data2).collect()
        truth = [t for t in input_2 if t[0] in input_1]
        assert sorted(res.astuples()) == truth

    def test_scalar_right(self, jitq_context):
        input_1 = list(enumerate(range(10)))
        input_2 = range(10)

        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)
        res = data1.join(data2).collect()
        truth = [t for t in input_1 if t[0] in input_2]
        assert sorted(res.astuples()) == truth

    def test_nonmatching_keys(self, jitq_context):
        input_1 = list(enumerate(range(10)))
        input_2 = [(float(a), b) for a, b in input_1]

        with pytest.raises(TypeError):
            jitq_context.collection(input_1) \
                .join(jitq_context.collection(input_2))

    def test_overlap(self, jitq_context):
        input_1 = [(r, r * 10) for r in range(10)]
        input_2 = [(r, r * 13, r + 100) for r in range(5, 15)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.join(data2).collect()
        truth = [(r, r * 10, r * 13, r + 100) for r in range(5, 10)]
        assert sorted(res.astuples()) == truth

    def test_left_single_field(self, jitq_context):
        input_1 = [(r,) for r in range(10)]
        input_2 = [(r, r * 13, r + 100) for r in range(5, 15)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.join(data2).collect()
        truth = [(r, r * 13, r + 100) for r in range(5, 10)]
        assert sorted(res.astuples()) == truth

    def test_right_single_field(self, jitq_context):
        input_1 = [(r, r * 10) for r in range(10)]
        input_2 = [(r,) for r in range(5, 15)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.join(data2).collect()
        truth = [(r, r * 10) for r in range(5, 10)]
        assert sorted(res.astuples()) == truth

    def test_repeated_keys(self, jitq_context):
        input_1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        joined = data1.join(data2)
        res = joined.collect()
        truth = [(1, 2, 22, 33), (1, 3, 22, 33), (1, 2, 44, 55),
                 (1, 3, 44, 55), (2, 4, 33, 44), (2, 6, 33, 44)]
        assert sorted(res.astuples()) == sorted(truth)


class TestFilter:

    def test_count_scalar(self, jitq_context):
        res = jitq_context.collection(range(0, 10)) \
            .filter(lambda w: w % 2 == 0) \
            .count()
        truth = len(list(filter(lambda w: w % 2 == 0, range(0, 10))))
        assert res == truth

    def test_collect_scalar(self, jitq_context):
        res = jitq_context.collection(range(0, 10)) \
            .filter(lambda w: w % 2 == 0) \
            .collect()
        truth = list(filter(lambda w: w % 2 == 0, range(0, 10)))
        assert sorted(res.astuples()) == truth

    def test_count_tuple(self, jitq_context):
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .filter(lambda w: w[0] % 2 == 0) \
            .count()
        truth = len(list(filter(lambda w: w[0] % 2 == 0, input_)))
        assert res == truth

    def test_collect_tuple(self, jitq_context):
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .filter(lambda w: w[0] % 2 == 0) \
            .collect()
        truth = list(filter(lambda w: w[0] % 2 == 0, input_))
        assert sorted(res.astuples()) == truth

    def test_constant_false(self, jitq_context):
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .filter(lambda t: False) \
            .collect()
        assert sorted(res.astuples()) == []

    def test_move_around(self, jitq_context):
        input_ = list(range(10))
        res = jitq_context.collection(input_) \
            .map(lambda t: (0, t)) \
            .filter(lambda t: t[1] % 2 == 0) \
            .collect()
        truth = [(0, 0), (0, 2), (0, 4), (0, 6), (0, 8)]
        assert sorted(res.astuples()) == truth

    def test_multiple_writers1(self, jitq_context):
        input_ = list(range(10))
        res = jitq_context.collection(input_) \
            .map(lambda t: (t, t + 1)) \
            .filter(lambda t: (t[0] + t[1]) % 3 == 0) \
            .collect()
        truth = [(1, 2), (4, 5), (7, 8)]
        assert sorted(res.astuples()) == truth

    def test_multiple_writers2(self, jitq_context):
        input_ = list(range(10))
        res = jitq_context.collection(input_) \
            .map(lambda t: (t, t)) \
            .filter(lambda t: t[0] + t[1] < 10) \
            .collect()
        truth = list(enumerate(range(5)))
        assert sorted(res.astuples()) == truth


class TestMap:

    def test_scalar_to_scalar(self, jitq_context):
        res = jitq_context.collection(range(0, 10)) \
            .map(lambda i: i + 2) \
            .collect()
        assert sorted(res.astuples()) == [i + 2 for i in range(0, 10)]

    def test_scalar_to_tuple(self, jitq_context):
        res = jitq_context.collection(range(0, 10)) \
            .map(lambda i: (i, (i, i * 10))) \
            .collect()
        truth = [(i, (i, i * 10)) for i in range(0, 10)]
        assert sorted(res.astuples()) == truth

    def test_tuple_to_scalar(self, jitq_context):
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .map(lambda t: t[0] + t[1]) \
            .collect()
        assert sorted(res.astuples()) == [t[0] + t[1] for t in input_]

    def test_tuple_to_tuple(self, jitq_context):
        input_ = list(enumerate(range(10)))
        res = jitq_context.collection(input_) \
            .map(lambda t: (t[0] * 2, t[1] * 2)) \
            .collect()
        truth = [(t[0] * 2, t[1] * 2) for t in input_]
        assert sorted(res.astuples()) == truth

    def test_count(self, jitq_context):
        res = jitq_context.collection(range(0, 10)) \
            .map(lambda t: (t, t * 10)) \
            .count()
        assert res == 10

    def test_record1(self, jitq_context):
        input_ = np.array([(1, 2)], dtype=[('field0', 'i8'), ('field1', 'i8')])
        res = jitq_context.collection(input_).map(lambda x: x.field1).collect()
        assert [2] == res.astuplelist()

    def test_record2(self, jitq_context):
        input_ = np.array([(1, 2)], dtype=[('field0', 'i8'), ('field1', 'i8')])
        res = jitq_context.collection(input_).map(lambda x: x).collect()
        assert res[0]["field0"] == 1
        assert res[0]["field1"] == 2

    def test_small_return_struct(self, jitq_context):
        # due to ABI {i4, i4} would be merged into i8 by clang if returned
        # by value
        # test that it does not happen
        input_ = np.array([(1, 2)], dtype="i4,i4")
        res1 = jitq_context.collection(input_).map(lambda x: x).collect()
        assert [(1 == 2)], res1.astuplelist()

    def test_inlining(self, jitq_context):
        # due to ABI {f4, f4} turns into <f4, f4> if returned
        # by value
        # the test happens to execute successfully, but the inline test should
        # fail
        input_ = np.array([(3, 8)], dtype="f4,f4")
        res1 = jitq_context.collection(input_).map(lambda x: x).collect()
        assert [(3 == 8)], res1.astuplelist()


class TestReduce:

    def test_sum(self, jitq_context):
        input_ = range(0, 10)
        data = jitq_context.collection(input_) \
            .reduce(lambda tuple_1, tuple_2: tuple_1 + tuple_2)
        assert data == sum(input_)

    def test_tuple(self, jitq_context):
        input_ = [(r, r * 10) for r in range(0, 10)]
        res = jitq_context.collection(input_) \
            .reduce(lambda tuple_1, tuple_2:
                    (tuple_1[0] + tuple_2[0], tuple_1[1] + tuple_2[1]))
        res = tuple(res)
        truth = (sum(map(lambda t: t[0], input_)),
                 sum(map(lambda t: t[1], input_)))
        assert res == truth

    def test_reduce_empty(self, jitq_context):
        res = jitq_context.range_(0, 0).reduce(lambda i1, i2: i1 + i2)
        assert res is None

    def test_count_empty(self, jitq_context):
        res = jitq_context.range_(0, 0).count()
        assert res == 0


class TestReduceByKey:

    def test_basic(self, jitq_context):
        input_ = [(0, 1), (1, 1), (1, 1), (0, 1), (1, 1)]
        data = jitq_context.collection(input_) \
            .reduce_by_key(lambda tuple_1, tuple_2: tuple_1 + tuple_2) \
            .collect()
        truth = {(0, 2), (1, 3)}
        assert set(data.astuples()) == truth

    def test_tuple(self, jitq_context):
        input_ = [(0, 1, 2), (1, 1, 2), (1, 1, 2), (0, 1, 2), (1, 1, 2)]
        data = jitq_context.collection(input_) \
            .reduce_by_key(lambda tuple_1, tuple_2:
                           (tuple_1[0] + tuple_2[0],
                            tuple_1[1] + tuple_2[1])) \
            .collect()
        truth = [(0, 2, 4), (1, 3, 6)]
        assert sorted(data.astuples()) == sorted(truth)

    def test_grouped(self, jitq_context):
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

        assert sorted(data.astuples()) == sorted(truth)


class TestCartesian:

    def test_count(self, jitq_context):
        input_1 = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77), (2, 33, 44)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        joined = data1.cartesian(data2)
        res = joined.count()
        truth = 20
        assert res == truth

    def test_tuple_tuple(self, jitq_context):
        input_1 = [(1, 2), (1, 3)]
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.cartesian(data2).collect()
        truth = {x + y for x in input_1 for y in input_2}
        assert set(res.astuples()) == truth

    def test_scalar_tuple(self, jitq_context):
        input_1 = list(range(5))
        input_2 = [(1, 22, 33), (1, 44, 55), (8, 66, 77)]
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.cartesian(data2).collect()
        truth = [(x,) + y for x in input_1 for y in input_2]
        assert sorted(res.astuples()) == sorted(truth)

    def test_tuple_scalar(self, jitq_context):
        input_1 = [(1, 2), (1, 3)]
        input_2 = list(range(5))
        data1 = jitq_context.collection(input_1)
        data2 = jitq_context.collection(input_2)

        res = data1.cartesian(data2).collect()
        truth = [x + (y,) for x in input_1 for y in input_2]
        assert sorted(res.astuples()) == sorted(truth)

    def test_scalar_scalar(self, jitq_context):
        input1 = jitq_context.collection(range(0, 2))
        input2 = jitq_context.collection(range(0, 2))
        res = input1.cartesian(input2).collect()
        truth = [(0, 0), (0, 1), (1, 0), (1, 1)]
        assert sorted(res.astuples()) == sorted(truth)


class TestCache:

    def test_map1(self, jitq_context):
        for _ in range(2):
            jitq_context.clear_caches()
            res = jitq_context.collection(range(0, 10)) \
                .map(lambda t: t + 1) \
                .collect()
            assert sorted(res.astuples()) == list(range(1, 11))

    def test_map2(self, jitq_context):
        res = jitq_context.collection(range(0, 10)) \
            .map(lambda t: t + 2) \
            .collect()
        assert sorted(res.astuples()) == list(range(2, 12))

    def test_filter1(self, jitq_context):
        for _ in range(2):
            jitq_context.clear_caches()
            res = jitq_context.collection(range(0, 10)) \
                .filter(lambda t: t % 2 == 0) \
                .collect()
            assert sorted(res.astuples()) == list(range(0, 10, 2))

    def test_filter2(self, jitq_context):
        res = jitq_context.collection(range(0, 10)) \
            .filter(lambda t: t % 2 == 1) \
            .collect()
        assert sorted(res.astuples()) == list(range(1, 10, 2))

    def test_reuse_input(self, jitq_context):
        input_ = jitq_context.collection(range(0, 10))
        res1 = input_.count()
        res2 = input_.count()
        assert res1 == res2

        # TODO(sabir): test cases for the caching mechanism of all operators


class TestSortedness:

    def test_index_grouped(self, jitq_context):
        res = jitq_context.collection(list(range(10)), add_index=True) \
            .reduce_by_key(lambda i1, i2: i1 + i2)\
            .collect()
        truth = list(enumerate(range(10)))
        assert sorted(res.astuples()) == truth

    def test_range_grouped(self, jitq_context):
        res = jitq_context.range_(0, 10) \
            .map(lambda i: (i, i)) \
            .reduce_by_key(lambda i1, i2: i1 + i2)\
            .collect()
        truth = list(enumerate(range(10)))
        assert sorted(res.astuples()) == truth

    def test_cartesian_grouped(self, jitq_context):
        res = jitq_context.collection(list(range(5))) \
            .cartesian(jitq_context.range_(0, 2)) \
            .map(lambda t: (t[1], t[0])) \
            .reduce_by_key(lambda i1, i2: i1 + i2)\
            .collect()
        truth = [(i, sum(range(5))) for i in range(2)]
        assert sorted(res.astuples()) == truth

    def test_join_right_grouped(self, jitq_context):
        res = jitq_context.range_(0, 10) \
            .map(lambda i: (int(i / 2), i)) \
            .join(jitq_context.range_(0, 10)) \
            .map(lambda t: (t[1], t[0])) \
            .reduce_by_key(lambda i1, i2: i1 + i2)\
            .collect()
        truth = [(i, int(i / 2)) for i in range(10)]
        assert sorted(res.astuples()) == truth

    def test_join_right_unique(self, jitq_context):
        res = jitq_context.range_(0, 10) \
            .map(lambda i: (i % 3, i % 5)) \
            .join(jitq_context.range_(0, 10)
                  .map(lambda i: (i % 5, 1))
                  .reduce_by_key(lambda i1, i2: i1 + i2)) \
            .map(lambda t: (t[1], 1)) \
            .reduce_by_key(lambda i1, i2: i1 + i2)\
            .collect()
        truth = [(i, 2) for i in range(5)]
        assert sorted(res.astuples()) == truth

    def test_join_key_grouped(self, jitq_context):
        res = jitq_context.range_(0, 10) \
            .map(lambda i: (i, int(i / 2))) \
            .join(jitq_context.range_(0, 10)) \
            .reduce_by_key(lambda i1, i2: i1 + i2)\
            .collect()
        truth = [(i, int(i / 2)) for i in range(10)]
        assert sorted(res.astuples()) == truth

    def test_join_left_grouped(self, jitq_context):
        res = jitq_context.range_(0, 10) \
            .map(lambda i: (i, int(i / 2))) \
            .join(jitq_context.range_(0, 10)) \
            .map(lambda t: (t[1], t[0])) \
            .reduce_by_key(lambda i1, i2: i1 + i2)\
            .collect()
        truth = [(i, i * 4 + 1) for i in range(5)]
        assert sorted(res.astuples()) == truth


class TestFilterPushDown:

    def test_push_down_bifurcation(self, jitq_context):
        input_ = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        res = jitq_context.collection(input_) \
            .join(jitq_context.collection(input_)) \
            .filter(lambda a: a[1] == a[2]) \
            .collect()
        truth = [(c, a, a) for c, a in input_]
        assert sorted(res.astuples()) == sorted(truth)

    def test_push_up(self, jitq_context):
        input_ = [(1, 2), (1, 3), (2, 4), (3, 5), (2, 6)]
        res = jitq_context.collection(input_) \
            .filter(lambda a: a[0] % 2 == 0) \
            .join(jitq_context.collection(input_)) \
            .collect()
        truth = [(2, 4, 4), (2, 4, 6), (2, 6, 4), (2, 6, 6)]
        assert sorted(res.astuples()) == sorted(truth)

    def test_dag1(self, jitq_context):
        input1 = jitq_context.collection(range(0, 2))
        result = input1.map(lambda t: t).filter(lambda i: i == 0) \
                       .cartesian(input1).collect()
        truth = [(0, 0), (0, 1)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_dag2(self, jitq_context):
        input1 = jitq_context.collection(range(0, 2))
        result = input1.cartesian(input1) \
                       .filter(lambda t: t[0] == 0) \
                       .collect()
        truth = [(0, 0), (0, 1)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_dag3(self, jitq_context):
        input1 = jitq_context.collection(range(0, 2))
        result = input1.cartesian(input1) \
                       .filter(lambda t: t[1] == 0) \
                       .collect()
        truth = [(0, 0), (1, 0)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_dag4(self, jitq_context):
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input1.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[0] == 2) \
                       .collect()
        truth = [(2, 3, 1)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_dag5(self, jitq_context):
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input1.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[1] == 2) \
                       .collect()
        truth = [(1, 2, 0)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_dag6(self, jitq_context):
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input1.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[2] == 2) \
                       .collect()
        truth = [(3, 4, 2)]
        assert sorted(result.astuples()) == sorted(truth)


class TestIntegration:

    def test_reexecute_dag(self, jitq_context):
        input_ = jitq_context.collection(range(0, 2))
        res1 = input_.collect()
        res2 = input_.collect()
        assert sorted(res1.astuples()) == sorted(res2.astuples())

    def test_reuse_and_modify_dag(self, jitq_context):
        input_ = jitq_context.collection(range(0, 2))
        res1 = input_.collect()
        res2 = input_.filter(lambda t: True).collect()
        assert sorted(res1.astuples()) == sorted(res2.astuples())

    def test_dag_explosion(self, jitq_context):
        input_ = range(0, 10)
        operator = jitq_context.collection(input_)
        for _ in range(10):
            operator = operator.join(operator)
        res = operator.collect()
        assert sorted(res.astuples()) == list(input_)

    def test_dag1(self, jitq_context):
        input1 = jitq_context.collection(range(0, 2))
        result = input1.cartesian(input1).collect()
        truth = [(0, 0), (0, 1), (1, 0), (1, 1)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_dag2(self, jitq_context):
        input1 = jitq_context.collection(range(0, 2)).filter(lambda t: True)
        result = input1.cartesian(input1).collect()
        truth = [(0, 0), (0, 1), (1, 0), (1, 1)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_map_filter_map(self, jitq_context):
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
        assert sorted(res.astuples()) == truth

    def test_join_input_order(self, jitq_context):
        input1 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        input2 = jitq_context.collection(range(0, 4)).map(lambda i: (i, i + 1))
        result = input1.join(input2.map(lambda t: (t[1], t[0]))) \
                       .filter(lambda t: t[1] == 2) \
                       .collect()
        truth = [(1, 2, 0)]
        assert sorted(result.astuples()) == sorted(truth)

    def test_all_operators(self, jitq_context):
        input_1 = jitq_context.range_(0, 10).map(lambda i: (i, 1))
        input_2 = jitq_context.collection(range(0, 10)) \
            .map(lambda i: (2 * i, i))
        input_3 = jitq_context.collection(range(0, 10)) \
            .map(lambda i: (1, 1))
        res = input_1.join(input_2) \
            .map(lambda t: (t[0], t[2])) \
            .reduce_by_key(lambda a, b: a + b) \
            .cartesian(input_3) \
            .filter(lambda t: t[0] % 3 == 0) \
            .map(lambda t: t[2]) \
            .reduce(lambda tuple_1, tuple_2: tuple_1 + tuple_2)
        assert res == 20


if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)
