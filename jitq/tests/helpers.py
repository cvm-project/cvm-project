import argparse
import sys
import unittest


def run_tests():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument(
        "-r", "--repeat", type=int, default=1,
        help="Repeat tests given number of times")
    args, unit_args = parser.parse_known_args()
    unit_args.insert(0, "placeholder")  # unittest ignores first arg
    for _ in range(args.repeat):
        was_successful = unittest.main(exit=False, argv=unit_args) \
            .result.wasSuccessful()
        if not was_successful:
            sys.exit(1)
