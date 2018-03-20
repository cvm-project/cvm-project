#!/usr/bin/env python3

from jitq.tests.helpers import run_tests

# pylint: disable=wildcard-import, unused-wildcard-import
# OK for test cases
from jitq.tests.operator_test_base import *


def set_up():
    JitqContext.default_conf = {"codegen": {"backend": "cpp"}}


if __name__ == '__main__':
    set_up()
    run_tests()
