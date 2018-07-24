#!/usr/bin/env python3

import glob
import shutil
import subprocess
import unittest

import re
from os.path import join, realpath, dirname, basename

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
        subprocess.run(cmd)


class TestInlining(unittest.TestCase):
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
        # remove previous compilation results
        shutil.rmtree(LIB_PATH)
        test_case = InliningTestCase(self, test_name)
        test_case.run_optimizer()
        files = glob.glob(join(LIB_PATH, "*.so"))
        for file_name in files:
            print("checking inlining in file " + file_name)
            print("test name: " + test_name)
            result = subprocess.run(
                ['objdump', '-D', file_name], stdout=subprocess.PIPE)
            file_content = str(result.stdout)
            pattern = re.compile(".*call.*_llvm_.*")
            for line in file_content.split("\\n"):
                if pattern.match(line):
                    raise self.failureException(
                        "Call to UDF function has not been inlined, "
                        "line " + line)

    return test_func


TestInlining.attach_test_cases()
if __name__ == '__main__':
    unittest.main()
