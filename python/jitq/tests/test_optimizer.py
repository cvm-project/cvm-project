#!/usr/bin/env python3

import argparse
import glob
import json
import subprocess
import unittest

from os.path import join, realpath, dirname, basename
from abc import ABC, abstractproperty

SCRIPT_DIR = realpath(dirname(__file__))
TEST_BASE_DIR = SCRIPT_DIR
TEST_EXE = join(dirname(dirname(dirname(SCRIPT_DIR))),
                "backend", "build", "opt")


def format_json(json_string):
    return json.dumps(json.loads(json_string), sort_keys=True, indent=4)


class OptimizerTestCase:
    def __init__(self, test_class, test_name):
        self.test_name = test_name
        self.test_class = test_class

    def input_file(self):
        return join(self.test_class.TEST_DIR, self.test_name + ".input.json")

    def output_file(self):
        return join(self.test_class.TEST_DIR, self.test_name + ".output.json")

    def read_input(self):
        with open(self.input_file()) as file_:
            return file_.read()

    def read_output(self):
        with open(self.output_file()) as file_:
            return file_.read()

    def write_output(self, output):
        with open(self.output_file(), 'w') as file_:
            return file_.write(output)

    def run_optimizer(self):
        cmd = [TEST_EXE] + self.test_class.TEST_OPTIONS + \
            ["-i", self.input_file()]
        output = subprocess.check_output(cmd)
        output = output.decode("utf-8")
        return output


class TestBase(ABC):
    @abstractproperty
    @classmethod
    # pylint: disable=invalid-name
    # This property should be treated as constant and is hence all caps.
    def TEST_DIR(cls):
        pass

    @classmethod
    def test_names(cls):
        input_files = glob.glob(join(cls.TEST_DIR, "*.input.json"))
        return (basename(f)[:-len(".input.json")] for f in input_files)

    # Attach a test case for each input file
    @classmethod
    def attach_test_cases(cls):
        for test_name in cls.test_names():
            setattr(cls, "test_" + test_name, make_test_func(test_name))

    @classmethod
    def write_outputs(cls):
        for test_name in cls.test_names():
            test_case = OptimizerTestCase(cls, test_name)
            output = test_case.run_optimizer()
            test_case.write_output(format_json(output))


# Test class instantiation for de/reserialization
class TestParser(unittest.TestCase, TestBase):
    TEST_DIR = join(TEST_BASE_DIR, "test_dag_parser")
    TEST_OPTIONS = []


# Test class instantiation for optimizer
class TestOptimizer(unittest.TestCase, TestBase):
    TEST_DIR = join(TEST_BASE_DIR, "test_optimizer")
    TEST_OPTIONS = ["-O1"]


# Factory for generic test cases
def make_test_func(test_name):
    # Implementation of generic test case
    def test_func(self):
        test_case = OptimizerTestCase(self, test_name)
        output = format_json(test_case.run_optimizer())
        ref = format_json(test_case.read_output())
        self.assertEqual(output, ref)

    return test_func


TEST_CLASSES = [TestParser, TestOptimizer]


def attach_test_cases():
    for cls in TEST_CLASSES:
        cls.attach_test_cases()


def main():
    parser = argparse.ArgumentParser(
        description='Test case for DAG parsing and optimizing.')
    parser.add_argument('--write_outputs', action='store_true',
                        help='Rewrite reference output files ' +
                             'based on current test results.')
    args, unit_args = parser.parse_known_args()

    if args.write_outputs:
        for cls in TEST_CLASSES:
            cls.write_outputs()
    else:
        unit_args.insert(0, "placeholder")  # unittest ignores first arg
        unittest.main(argv=unit_args)


attach_test_cases()
if __name__ == '__main__':
    main()
