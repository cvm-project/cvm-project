#!/usr/bin/env python3

import glob
import json
import os
from os.path import join, realpath, dirname, basename
import subprocess
import unittest

SCRIPT_DIR = realpath(dirname(__file__))
TEST_DIR = join(SCRIPT_DIR, "test_dag_parser")
TEST_EXE = join(
    dirname(dirname(SCRIPT_DIR)), "cpp", "build", "dag_parser_tests")


# Factory for generic test cases
def make_test_func(test_name):
    # Implementation of generic test case
    def test_func(self):
        testfile = join(TEST_DIR, test_name + ".input.json")

        with open(testfile) as file_:
            ref = file_.read()
        ref = json.dumps(json.loads(ref), sort_keys=True)

        output = subprocess.check_output([TEST_EXE, "-i", testfile])
        output = output.decode("utf-8")
        output = json.dumps(json.loads(output), sort_keys=True)

        self.assertEqual(output, ref)

    return test_func


class TestParser(unittest.TestCase):
    pass


# Attach a test case for each input file
def attach_test_cases():
    for file_ in glob.glob(TEST_DIR + os.sep + "*.input.json"):
        test_name = basename(file_)[:-len(".input.json")]
        setattr(TestParser, "test_" + test_name, make_test_func(test_name))


if __name__ == '__main__':
    attach_test_cases()
    unittest.main()
