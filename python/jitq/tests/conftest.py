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
        '--tpch_print_result', action='store_true',
        help='Print result after running TPC-H test cases.')
    parser.addoption(
        '--tpch_scale', action='store', default='0.001m',
        help='TPC-H Scale Factor.')
    parser.addoption(
        '--tpch_input_format', action='append', default=[],
        help='Input format of TPC-H data.')
    parser.addoption(
        '--tpch_input_path', action='store',
        default='{jitqpath}/python/jitq/tests/tpch/'
                '{format}-{sf}/{table}.{format}',
        help='Format string for path to input files.')
    parser.addoption(
        '--tpch_num_partitions', action='store', default=1, type=int,
        help='Number of files in partitioned input relations.')
    parser.addoption(
        '--tpch_ref_path', action='store',
        default='{jitqpath}/python/jitq/tests/tpch/ref-{sf}/Q{q}.ref.pkl',
        help='Format string for path to reference result.')
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

    if 'tpch_print_result' in metafunc.fixturenames:
        tpch_print_result = [metafunc.config.option.tpch_print_result]
        metafunc.parametrize('tpch_print_result',
                             tpch_print_result,
                             scope='session')

    if 'tpch_scale' in metafunc.fixturenames:
        tpch_scale = [metafunc.config.option.tpch_scale]
        metafunc.parametrize('tpch_scale',
                             tpch_scale,
                             scope='session')

    if 'tpch_input_format' in metafunc.fixturenames:
        tpch_input_format = metafunc.config.option.tpch_input_format or \
            ['pandas/csv']
        tpch_input_format = list(set(tpch_input_format))
        metafunc.parametrize('tpch_input_format',
                             tpch_input_format,
                             scope='session')

    if 'tpch_input_path' in metafunc.fixturenames:
        tpch_input_path = [metafunc.config.option.tpch_input_path]
        metafunc.parametrize('tpch_input_path',
                             tpch_input_path,
                             scope='session')

    if 'tpch_num_partitions' in metafunc.fixturenames:
        tpch_num_partitions = [metafunc.config.option.tpch_num_partitions]
        metafunc.parametrize('tpch_num_partitions',
                             tpch_num_partitions,
                             scope='session')

    if 'tpch_ref_path' in metafunc.fixturenames:
        tpch_ref_path = [metafunc.config.option.tpch_ref_path]
        metafunc.parametrize('tpch_ref_path',
                             tpch_ref_path,
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
    filename_parts = []

    test_class = request.node.getparent(pytest.Class)
    if test_class:
        filename_parts.append(test_class.name)

    test_function = request.node.getparent(pytest.Function)
    if test_function:
        filename_parts.append(test_function.name)

    filename_base = '_'.join(filename_parts) \
        .translate(str.maketrans("/{}[]-", "______"))

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
