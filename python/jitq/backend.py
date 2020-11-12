# This is a lazy way to get the compiled Python module into jitq.backend
# without setting up a ful packaging mechanism, which looks complicated
# (see https://github.com/pybind/pybind11/issues/1430).

# pylint: disable=import-error
#         This saves us building the backend to do the Python format check
# pylint: disable=no-name-in-module
#         pylint seems unable to see the functions of our native module
# pylint: disable=unused-import
#         The imported symbols are used by other modules
from jitq_backend import \
    DumpDag, \
    ExecutePlan, \
    FreeResult, \
    GenerateExecutable
