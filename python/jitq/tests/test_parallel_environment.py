#!/usr/bin/env python3


def test_num_threads(jitq_context):
    import psutil

    # Run some job; this should spawn threads, if possible
    jitq_context.range_(0, 10).collect()

    # Make sure that this process uses some additional threads (for OpenMP)
    # (If this test fails, the tests are probably run on a system with a
    # single CPU)
    assert psutil.Process().num_threads() > 1


if __name__ == '__main__':
    import sys
    import pytest
    pytest.main(sys.argv)
