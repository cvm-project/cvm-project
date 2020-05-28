# pylint: disable=redefined-outer-name  # required by pytest
# pylint: disable=unused-import         # pytest needs to see some symbols

import os

import pytest

from jitq.jitq_context import JitqContext
from jitq.tests.filesystem import FILESYSTEMS, filesystem_instance
from jitq.tests.helpers import str2bool


def pytest_addoption(parser):
    parser.addoption(
        '-T', '--target', action='append', default=[],
        help='Run the tests on target platform(s).')
    parser.addoption(
        '-F', '--filesystem', action='append', default=[],
        help='Select filesystem on which file-based test should run.')
    parser.addoption(
        '--s3_bucket_name', action='store', default='test-bucket',
        help='Bucket name on S3 that should be used for input/output data.')
    parser.addoption(
        '-G', '--generate_testcases', action='store_true',
        help='Generate test input files for the optimizer and parser.')
    parser.addoption(
        '--write_outputs', action='store_true',
        help='Rewrite reference output files of optimizer test '
             'based on current test results.')


def pytest_generate_tests(metafunc):
    if 'target' in metafunc.fixturenames:
        target = metafunc.config.option.target or ['singlecore', 'omp']
        target = list(set(target))
        metafunc.parametrize('target',
                             target,
                             scope='session')

    if 'filesystem' in metafunc.fixturenames:
        filesystem = metafunc.config.option.filesystem or FILESYSTEMS.keys()
        filesystem = list(set(filesystem))
        metafunc.parametrize('filesystem',
                             filesystem,
                             scope='session')

    if 'write_outputs' in metafunc.fixturenames:
        write_outputs = \
            [metafunc.config.option.write_outputs]
        metafunc.parametrize('write_outputs',
                             write_outputs,
                             scope='session')

    if 'generate_testcases_enabled' in metafunc.fixturenames:
        generate_testcases = \
            [metafunc.config.option.generate_testcases]
        metafunc.parametrize('generate_testcases_enabled',
                             generate_testcases,
                             scope='session')


@pytest.fixture
def generate_testcases(generate_testcases_enabled, request):
    if not generate_testcases_enabled:
        yield
        return

    from jitq.utils import get_project_path
    from jitq.tests.test_optimizer import OPTIMIZER_CONFIGS
    from jitq.tests.helpers import format_json

    dag_filename = os.path.join(get_project_path(), 'dag.json')

    # Remove old DAG
    try:
        os.remove(dag_filename)
    except BaseException:
        pass

    # Run test
    yield

    if not os.path.isfile(dag_filename):
        pytest.xfail('No DAG produced')

    # Compute file name to parser test case
    class_name = request.node.getparent(pytest.Class).name
    function_name = request.node.getparent(pytest.Function).name
    filename_base = '_'.join([class_name, function_name]) \
        .translate(str.maketrans("[]-", "___"))

    # Read generated DAG and write it for each class
    for config in OPTIMIZER_CONFIGS.values():
        target_filename = os.path.join(config.directory,
                                       filename_base + '.input.json')
        with open(dag_filename, 'r') as input_file:
            with open(target_filename, 'w') as output_file:
                output_file.write(format_json(input_file.read()))


@pytest.fixture
def jitq_context(target, generate_testcases):
    # pylint: disable=unused-argument  # needed as fixture
    return JitqContext(conf={
        'optimizer': {
            'target': target,
        },
    })
