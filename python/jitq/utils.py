from json import JSONEncoder
import os
import sys
import time

import numba as nb
from numba import typeof
import numpy as np


def mean(lst):
    return sum(lst) / len(lst)


def timer(func, max_rep=3):
    res = []
    for _ in range(0, max_rep):
        time_1 = time.perf_counter()
        resu = func()
        time_2 = time.perf_counter()
        res.append(time_2 - time_1)
        print(resu)
    return mean(res)


C_TYPE_MAP = {
    'float32': 'float',
    'float64': 'double',
    'int32': 'int',
    'int64': 'long',
    'boolean': 'bool',
    'bool': 'bool',
}


def numba_to_c_types(numba_type):
    numba_type = str(numba_type)
    for k, val in C_TYPE_MAP.items():
        numba_type = numba_type.replace(k, val)
    return numba_type


NUMPY_DTYPE_MAP = {
    'float32': 'f4',
    'float64': 'f8',
    'int32': 'i4',
    'int64': 'i8',
    'boolean': 'b1',
    'bool': 'b1',
}


def is_item_type(type_):
    if str(type_) in NUMPY_DTYPE_MAP:
        return True
    if not isinstance(type_, nb.types.Tuple):
        return False
    for child_type in type_.types:
        if not is_item_type(child_type):
            return False
    return True


def numba_type_to_dtype(type_):
    assert is_item_type(type_), "Expected valid nested tuple type."
    if isinstance(type_, nb.types.Tuple):
        child_types = [numba_type_to_dtype(t) for t in type_.types]
        fields = [('f%i' % i, t) for i, t in enumerate(child_types)]
        return np.dtype(fields)
    return np.dtype(NUMPY_DTYPE_MAP[type_.name])


def dtype_to_numba(type_):
    if type_.fields:
        # composite type
        types = []
        for _, val in type_.fields.items():
            numba_type = dtype_to_numba(val[0])
            types.append(numba_type)
        return "(" + ",".join(types) + ")"
    return typeof(type_.name)


def get_type_size(type_):
    size = 0
    try:
        size = int(type_.bitwidth / 8)
    except AttributeError:
        if isinstance(type_, nb.types.Tuple):
            for child_type in type_.types:
                size += get_type_size(child_type)
        elif isinstance(type_, nb.types.UniTuple):
            size = get_type_size(type_.dtype) * type_.count
    return size


class RDDEncoder(JSONEncoder):

    # pylint: disable=method-hidden
    # sabir 14.02.18: JSONEncoder overwrites this method, nothing we can do
    def default(self, o):
        if isinstance(o, nb.types.Type):
            return numba_to_c_types(o.name)
        return JSONEncoder.default(self, o)


def error_print(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def item_typeof(expression):
    if isinstance(expression, np.ndarray):
        return item_typeof(tuple(expression))
    return replace_unituple(typeof(expression))


def make_tuple(child_types):
    out = nb.types.Tuple([])
    out.types = tuple(child_types)
    out.name = "(%s)" % ', '.join(str(i) for i in child_types)
    out.count = len(child_types)
    assert is_item_type(out), "Expected valid nested tuple type."
    return out


def replace_unituple(type_):
    if isinstance(type_, nb.types.BaseAnonymousTuple):
        child_types = [replace_unituple(t) for t in type_.types]
        return make_tuple(child_types)
    if str(type_) in NUMPY_DTYPE_MAP:
        return type_
    raise TypeError("Can only replace UniTuple on valid nested tuples.")


def get_project_path():
    path = ""
    try:
        path = os.environ['JITQPATH']
    except KeyError:
        error_print(
            "JITQPATH is not defined, set it to your jitq installation path")
        exit(1)
    return path


def flatten(iterable_):
    """
    Flatten nested iterable of (tuple, list).
    """

    def rec(iterable__):
        for i in iterable__:
            if isinstance(i, (tuple, list)):
                for j in rec(i):
                    yield j
            elif isinstance(i, (nb.types.UniTuple, nb.types.Tuple)):
                for j in rec(i.types):
                    yield j
            else:
                yield i

    if not isinstance(iterable_,
                      (tuple, list, nb.types.UniTuple, nb.types.Tuple)):
        return iterable_,
    return tuple(rec(iterable_))


class Timer:
    def __init__(self):
        self._start = 0
        self._end = 0

    def start(self):
        self._start = time.perf_counter() * 1000

    def end(self):
        self._end = time.perf_counter() * 1000

    def diff(self):
        return str(self._end - self._start)


def measure_time(func, max_rep=3, show_runs=False):
    res = []
    for i in range(0, max_rep):
        time_1 = time.perf_counter()
        func()
        time_2 = time.perf_counter()
        res.append(time_2 - time_1)
        if show_runs:
            print("run " + str(i) + " " + str(time_2 - time_1))
    return mean(res) * 1000
