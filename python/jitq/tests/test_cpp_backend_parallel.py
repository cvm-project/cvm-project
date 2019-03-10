#!/usr/bin/env python3

import unittest
import psutil

from jitq.tests.helpers import run_tests, load_and_patch_common_tests, \
    load_tests_common_patched
from jitq.jitq_context import JitqContext


def set_up(self):
    self.context = JitqContext(conf={"optimizer": {"optimizations": {
        "parallelize": {"active": True},
        "code_gen": {"backend": "cpp"}
    }}})


class TestParallelTestEnvironment(unittest.TestCase):
    def test_scalar(self):
        # Run some job; this should spawn threads, if possible
        JitqContext().range_(0, 10).collect()

        # Make sure that this process uses some additional threads (for OpenMP)
        # (If this test fails, the tests are probably run on a system with a
        # single CPU)
        self.assertGreater(psutil.Process().num_threads(), 1)


NAME_SUFFIX = "CppParallel"
FUNCTIONS = {"setUp": set_up}


def load_tests(loader, _standard_tests, _pattern):
    return load_tests_common_patched(
        loader, NAME_SUFFIX, FUNCTIONS)


load_and_patch_common_tests(FUNCTIONS)
if __name__ == '__main__':
    run_tests()
