#!/usr/bin/env python3

# pylint: disable=redefined-outer-name  # required by pytest

import random
import sys
from unittest import mock

import pyarrow.parquet as pq
import pytest

import jitq.backend
from jitq.serverless.driver import Driver
from jitq.tests.helpers import only_targets


@only_targets(['singlecore'])
def test_lambda_runner(jitq_aws_stack, jitq_context):
    # Only run with a single worker, as the file name of the Parquet output
    # is the same for all workers and several of them would overwrite each
    # others results.
    num_workers = 1

    def mock_execute_plan(plan_id, inputs_str):
        compiled_dag_str = jitq.backend.DumpDag(plan_id)
        inputs = [inputs_str] * num_workers

        with Driver(jitq_aws_stack) as driver:
            result = driver.run_query(compiled_dag_str, num_workers, inputs)
            assert len(result) == num_workers
            return result[0]

    with mock.patch('jitq.backend.ExecutePlan', mock_execute_plan):
        filename = '/tmp/jitq-test-lambda-runner-{:08x}.parquet' \
            .format(random.getrandbits(64))
        column_names = ['a', 'b']
        res = jitq_context.range_(0, 5) \
            .join(jitq_context.range_(0, 10)) \
            .join(jitq_context.range_(0, 15)) \
            .map(lambda i: (i % 2, i)) \
            .to_parquet(filename, column_names)
        table = pq.read_table(res)
        assert column_names == [c.name for c in table.columns]
        res = zip(*[c.to_pylist() for c in table.columns])
        assert [(0, 0), (0, 2), (0, 4), (1, 1), (1, 3)] == sorted(res)


if __name__ == '__main__':
    pytest.main(sys.argv)
