#!/usr/bin/env python3

import glob
import json
import os
import subprocess
import unittest

from os.path import join, realpath, dirname, basename

scriptdir = realpath(dirname(__file__))
testdir = join(scriptdir, "test_dag_parser")
testexe = join(dirname(dirname(scriptdir)), "cpp", "build", "dag_parser_tests")

# Factory for generic test cases
def make_test_func(test_name):
    # Implemnetation of generic test case
    def test_func(self):
        nonlocal test_name
        testfile = join(testdir, test_name + ".input.json")

        with open(testfile) as f:
            ref = f.read()
        ref = json.dumps(json.loads(ref), sort_keys=True)

        output = subprocess.check_output([testexe, "-i", testfile])
        output = output.decode("utf-8")
        output = json.dumps(json.loads(output), sort_keys=True)

        self.assertEqual(output, ref)
    return test_func

class TestParser(unittest.TestCase):
    pass

# Attach a test case for each input file
for f in glob.glob(testdir + os.sep + "*.input.json"):
    test_name = basename(f)[:-len(".input.json")]
    setattr(TestParser, "test_" + test_name, make_test_func(test_name))

if __name__ == '__main__':
    unittest.main()
