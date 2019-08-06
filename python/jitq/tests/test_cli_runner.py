#!/usr/bin/env python3

import logging
import os
import subprocess
from unittest import mock

import pytest

import jitq.backend
from jitq.utils import get_project_path


def test_runner(jitq_context):
    logger = logging.getLogger(__name__)

    def mock_execute_plan(plan_id, inputs_str):
        dag_path = os.path.join(get_project_path(), 'compiled_dag.json')
        with open(dag_path, 'w') as fp:
            compiled_dag_str = jitq.backend.DumpDag(plan_id)
            fp.write(compiled_dag_str)

        env = os.environ.copy()
        env['PATH'] = os.path.join(os.environ['JITQPATH'],
                                   'backend', 'build') + \
            ':' + os.environ.get('PATH', '')

        try:
            output = subprocess \
                .check_output(['runner', '--dag', dag_path],
                              env=env, input=inputs_str.encode('utf-8'))

        except subprocess.CalledProcessError as ex:
            logger.error(
                'Fake lambda runtime failed. '
                'Output on stderr:\n%s\nOutput on stdout:\n%s',
                ex.stderr.decode('utf-8'),
                ex.stdout.decode('utf-8'))
            raise ex

        return output.decode('utf-8')

    with mock.patch('jitq.backend.ExecutePlan', mock_execute_plan):
        res = jitq_context.range_(0, 5) \
            .join(jitq_context.range_(0, 10)) \
            .join(jitq_context.range_(0, 15)) \
            .reduce(lambda i1, i2: i1 + i2)
        assert res == 10


if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)
