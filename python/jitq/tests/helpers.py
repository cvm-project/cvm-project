import argparse
import json
import os
import sys
import unittest

from jitq.utils import get_project_path


def format_json(json_string):
    return json.dumps(json.loads(json_string), sort_keys=True, indent=4)


def enable_dag_dumping():
    def augment_with_dag_dumping_power(self):
        # pylint: disable=protected-access
        # no better way to get this
        func = self._testMethodName
        cls = type(self).__name__
        # read generated json
        inp = get_project_path() + "/dag.json"
        from jitq.tests.test_optimizer import TEST_CLASSES
        for t_cl in TEST_CLASSES:
            out = t_cl.TEST_DIR + "/" + cls + "_" + func + ".input.json"
            with open(inp, 'r')as input_file:
                with open(out, 'w') as output_file:
                    output_file.write(format_json(input_file.read()))

    # pylint: disable=unused-argument
    def remove_old_dag_json(self):
        try:
            os.remove(get_project_path() + "/dag.json")
        except BaseException:
            pass

    def tear_down_wrapper(func):
        def inner(self):
            func(self)
            augment_with_dag_dumping_power(self)

        return inner

    def setup_wrapper(func):
        def inner(self):
            func(self)
            remove_old_dag_json(self)

        return inner

    test_suites = unittest.loader.defaultTestLoader.loadTestsFromModule(
        __import__("__main__"))
    for suit in test_suites:
        for test in suit:
            cls = test.__class__
            if cls.tearDown:
                cls.tearDown = tear_down_wrapper(cls.tearDown)
            else:
                cls.tearDown = augment_with_dag_dumping_power
            if cls.setUp:
                cls.setUp = setup_wrapper(cls.setUp)
            else:
                cls.setUp = remove_old_dag_json


def run_tests():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument(
        "-r", "--repeat", type=int, default=1,
        help="Repeat tests given number of times")
    parser.add_argument(
        "-g", "--generate", action="store_true",
        help="Generate test input files for the optimizer and parser."
    )
    args, unit_args = parser.parse_known_args()
    if args.generate:
        enable_dag_dumping()
    unit_args.insert(0, "placeholder")  # unittest ignores first arg
    for _ in range(args.repeat):
        was_successful = unittest.main(exit=False, argv=unit_args) \
            .result.wasSuccessful()
        if not was_successful:
            sys.exit(1)
