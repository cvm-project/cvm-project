#!/usr/bin/env python3

from jitq.tests import operator_test_base
from jitq.tests.helpers import run_tests

from jitq.jitq_context import JitqContext


class TestOperators(operator_test_base.TestOperators):
    def setUp(self):
        JitqContext.default_conf = {"codegen": {"backend": "cpp"}}


if __name__ == '__main__':
    run_tests()
