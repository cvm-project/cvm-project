#!/usr/bin/env python3

import glob
import subprocess
from collections import namedtuple
from os.path import join, realpath, dirname, basename

import pytest

from jitq.tests.helpers import format_json

SCRIPT_DIR = realpath(dirname(__file__))
TEST_BASE_DIR = SCRIPT_DIR
TEST_EXE = join(dirname(dirname(dirname(SCRIPT_DIR))),
                "backend", "build", "opt")


class OptimizerTestCase:
    def __init__(self, config, name):
        self.name = name
        self.config = config

    def _input_file(self):
        return join(self.config.directory, self.name + ".input.json")

    def _output_file(self):
        return join(self.config.directory, self.name + ".output.json")

    def read_input(self):
        with open(self._input_file()) as file_:
            return file_.read()

    def read_output(self):
        with open(self._output_file()) as file_:
            return file_.read()

    def write_output(self, output):
        with open(self._output_file(), 'w') as file_:
            return file_.write(output)

    def run_optimizer(self):
        cmd = [TEST_EXE] + self.config.options + \
            ["-i", self._input_file()]

        # Produce DOT output
        opt = subprocess.Popen(cmd + ['-fDOT'],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
        stdout, stderr = opt.communicate()
        status = opt.wait()

        if status != 0:
            raise RuntimeError("Error producing dot file:\n" +
                               stderr.decode('utf-8'))

        # Parse by dot
        dot = subprocess.Popen('dot', stdin=subprocess.PIPE,
                               stdout=subprocess.DEVNULL,
                               stderr=subprocess.PIPE)
        _, stderr = dot.communicate(stdout)
        status = dot.wait()

        if status != 0:
            raise RuntimeError("Error parsing produced dot file:\n" +
                               stderr.decode('utf-8'))

        # Produce JSON output and return
        output = subprocess.check_output(cmd)
        output = output.decode("utf-8")

        return output


OptimizerConfig = namedtuple('OptimizerConfig', ['directory', 'options'])


OPTIMIZER_CONFIGS = {
    'parser': OptimizerConfig(      # De/reserialization
        join(TEST_BASE_DIR, 'test_dag_parser'),
        ['-O0']),
    'o1': OptimizerConfig(          # Optimizer with '-O1'
        join(TEST_BASE_DIR, 'test_optimizer_o1'),
        ['-O1']),
    'o2': OptimizerConfig(           # Optimizer with '-O2'
        join(TEST_BASE_DIR, 'test_optimizer_o2'),
        ['-O2']),
    'omp': OptimizerConfig(  # Optimizer with '-O2' and omp target
        join(TEST_BASE_DIR, 'test_optimizer_omp'),
        ['-O2', '-Tomp']),
    'process': OptimizerConfig(  # Optimizer with '-O2' and process target
        join(TEST_BASE_DIR, 'test_optimizer_process'),
        ['-O2', '-Tprocess']),
}


def list_test_names(config):
    input_files = glob.glob(join(config.directory, '*.input.json'))
    return (basename(f)[:-len('.input.json')] for f in input_files)


@pytest.mark.parametrize('test_name',
                         list_test_names(OPTIMIZER_CONFIGS['parser']))
def test_parser_sanity(test_name):
    parser_config = OPTIMIZER_CONFIGS['parser']
    test_case = OptimizerTestCase(parser_config, test_name)
    input_ = test_case.read_input()
    output = test_case.read_output()
    assert input_ == output


@pytest.mark.parametrize(
    ['config_name', 'test_name'],
    [(config_name, name)
     for config_name, config in OPTIMIZER_CONFIGS.items()
     for name in list_test_names(config)])
def test_optimizer(config_name, test_name, write_outputs):
    config = OPTIMIZER_CONFIGS[config_name]
    test_case = OptimizerTestCase(config, test_name)
    output = format_json(test_case.run_optimizer())

    if write_outputs:
        test_case.write_output(output)
    else:
        ref = format_json(test_case.read_output())
        assert output == ref


if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)
