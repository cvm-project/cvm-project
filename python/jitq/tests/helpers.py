import argparse
import json
import os
import sys
import unittest

from jitq.utils import get_project_path
from jitq.tests import operator_test_base


def format_json(json_string):
    return json.dumps(json.loads(json_string), sort_keys=True, indent=4)


def operator_test_base_classes():
    suites = unittest.loader.defaultTestLoader.loadTestsFromModule(
        operator_test_base)

    classes = set()
    for suite in suites:
        for test in suite:
            classes.add(test.__class__)

    return classes


def patch_class(class_, functions):
    def wrapper(class_, func_name, new_f):
        existing_f = getattr(class_, func_name)

        def func(*args, **kwargs):
            existing_f(*args, **kwargs)
            new_f(*args, **kwargs)
        return func

    for func_name, func in functions.items():
        setattr(class_, func_name, wrapper(class_, func_name, func))


def patch_operator_test_cases(name_suffix, functions):
    classes = []
    for test_class in operator_test_base_classes():
        class_name = test_class.__name__ + name_suffix
        test_class = type(class_name, (test_class,), {})
        patch_class(test_class, functions)
        classes.append(test_class)
    return classes


# Loads patched test cases into calling module
def load_and_patch_common_tests(functions):
    for test_class in patch_operator_test_cases("", functions):
        setattr(sys.modules['__main__'], test_class.__name__, test_class)


# Loads patched test cases using unittest's 'load_tests' protocol for test
# discovery
def load_tests_common_patched(loader, name_suffix, functions):
    suite = unittest.TestSuite()
    for test_class in patch_operator_test_cases(name_suffix, functions):
        tests = loader.loadTestsFromTestCase(test_class)
        suite.addTests(tests)
    return suite


def enable_dag_dumping():
    def augment_with_dag_dumping_power(self):
        # pylint: disable=protected-access
        # no better way to get this
        func = self._testMethodName
        cls = type(self).__name__
        from jitq.tests.test_cpp_backend import NAME_SUFFIX
        cls = cls[:len(cls) - len(NAME_SUFFIX)]
        # read generated json
        inp = get_project_path() + "/dag.json"
        from jitq.tests.test_optimizer import TEST_CLASSES
        for t_cl in TEST_CLASSES:
            out = t_cl.TEST_DIR + "/" + cls + "_" + func + ".input.json"
            with open(inp, 'r')as input_file:
                with open(out, 'w') as output_file:
                    output_file.write(format_json(input_file.read()))

    def remove_old_dag_json(_):
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
    for cls in operator_test_base_classes():
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
