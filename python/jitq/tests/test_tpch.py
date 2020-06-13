#!/usr/bin/env python3

import numpy as np
import pandas as pd
import pytest

from jitq.tests.tpch.pandas import PandasCsv, PandasParquet
from jitq.tests.tpch.queries import Q01, Q04, Q06, Q12, Q14, Q17, Q19
from jitq.utils import get_project_path


pd.options.display.float_format = '{:.10g}'.format


@pytest.fixture
def tpch_data(tpch_input_path, tpch_input_format, tpch_scale, jitq_context):
    formats = {
        'pandas/csv': PandasCsv,
        'pandas/parquet': PandasParquet,
    }
    loader = formats[tpch_input_format]
    database = loader(tpch_input_path, tpch_scale, jitq_context)
    return database


def _upcast(df):
    for column in df.columns.values:
        if df[column].dtype not in [np.uint8, np.int8, np.uint16, np.int16,
                                    np.uint32, np.int32]:
            continue
        df[column] = df[column].astype(np.int64)


@pytest.mark.parametrize('tpch_query', [Q01, Q04, Q06, Q12, Q14, Q17, Q19])
def test_tpch(jitq_context, tpch_data, tpch_query, tpch_print_result,
              tpch_scale, tpch_ref_path, write_outputs):
    # pylint: disable=redefined-outer-name  # That's how fixtures work

    class_name = tpch_query.__name__

    # Load data and run query
    query = tpch_query(jitq_context)
    database = query.load(tpch_data)
    res = query.run(database)

    # Get postprocessed data frame
    res = query.postprocess(res).reset_index(drop=True)
    _upcast(res)

    ref_filename = tpch_ref_path \
        .format(jitqpath=get_project_path(), sf=tpch_scale, q=class_name[1:])

    # Print result
    if tpch_print_result:
        print(res)

    # Overwrite reference result
    if write_outputs:
        res.to_pickle(ref_filename)

    # Load reference result
    ref = pd.read_pickle(ref_filename).reset_index(drop=True)
    _upcast(ref)

    # Test correctness
    pd.util.testing.assert_frame_equal(res, ref)


if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)
