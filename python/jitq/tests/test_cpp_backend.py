#!/usr/bin/env python3

from jitq.tests.helpers import run_tests, load_and_patch_common_tests, \
    load_tests_common_patched
from jitq.jitq_context import JitqContext


def set_up():
    JitqContext.default_conf = {"codegen": {"backend": "cpp"}}


NAME_SUFFIX = "Cpp"
FUNCTIONS = {"setUp": lambda self: set_up()}


def load_tests(loader, _standard_tests, _pattern):
    return load_tests_common_patched(
        loader, NAME_SUFFIX, FUNCTIONS)


load_and_patch_common_tests(FUNCTIONS)
if __name__ == '__main__':
    run_tests()
