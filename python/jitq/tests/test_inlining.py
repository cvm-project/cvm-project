#!/usr/bin/env python3

import glob
import json
import re
import subprocess
import sys
from os.path import join, realpath, dirname, basename

import pytest

from jitq.utils import get_project_path

SCRIPT_DIR = realpath(dirname(__file__))
TEST_BASE_DIR = SCRIPT_DIR
LIB_PATH = join(get_project_path(), "backend", "gen")
TEST_EXE = join(dirname(dirname(dirname(SCRIPT_DIR))),
                "backend", "build", "opt")


class InliningTestCase:
    def __init__(self, test_class, test_name):
        self.test_name = test_name
        self.test_class = test_class

    def input_file(self):
        return join(self.test_class.TEST_DIR, self.test_name + ".input.json")

    def run_optimizer(self):
        cmd = [TEST_EXE] + self.test_class.TEST_OPTIONS + \
            ["-i", self.input_file(), "-f", "BIN"]
        return subprocess.check_output(cmd, universal_newlines=True)


class TestInlining:
    TEST_DIR = join(TEST_BASE_DIR, "test_dag_parser")
    TEST_OPTIONS = ["-O2", "-tadd-always-inline"]

    @classmethod
    def input_files(cls):
        return glob.glob(join(cls.TEST_DIR, "*.input.json"))

    @classmethod
    def test_names(cls):
        return (basename(f)[:-len(".input.json")] for f in cls.input_files())

    # Attach a test case for each input file
    @classmethod
    def attach_test_cases(cls):
        for test_name in cls.test_names():
            setattr(cls, "test_" + test_name, make_test_func(test_name))


# Factory for generic test cases
def make_test_func(test_name):
    # Implementation of generic test case
    def test_func(self):
        test_case = InliningTestCase(self, test_name)
        output = test_case.run_optimizer()
        output = json.loads(output)
        files = [o['library_name'] for o in output['operators']
                 if 'library_name' in o]
        for file_name in files:
            file_name = join(LIB_PATH, file_name)
            print("checking inlining in file " + file_name)
            result = subprocess.run(
                ['objdump', '-D', file_name], stdout=subprocess.PIPE)
            file_content = str(result.stdout)
            pattern = re.compile(".*call.*_llvm_.*")
            for line in file_content.split("\\n"):
                assert not pattern.match(line), \
                    "Call to UDF function has not been inlined, line " + line

    return test_func


TestInlining.attach_test_cases()
if __name__ == '__main__':
    pytest.main(sys.argv)
