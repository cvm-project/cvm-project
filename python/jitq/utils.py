from json import JSONEncoder
import os
import sys
import time
from collections import namedtuple

import numba as nb
import numpy as np

# pylint: disable=unused-import
#         We need to load the native module first before modifying the env
import jitq.backend


def fix_preload():
    # After importing the native module, remove the Clang address sanitizer
    # from LD_PRELOAD (if we are using it); otherwise, external processes will
    # be called with through the sanitizer, which will most likely not work.
    if 'LD_PRELOAD' in os.environ:
        os.environ['JITQ_LD_PRELOAD'] = os.environ['LD_PRELOAD']
        asan_path = os.environ.get('ASAN_LIBRARY_PATH', '')
        preloads = []
        for path in os.environ['LD_PRELOAD'].split(' '):
            if 'dlclose_noop' not in path and path != asan_path:
                preloads.append(path)
        os.environ['LD_PRELOAD'] = ' '.join(preloads)


fix_preload()


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
    'unicode_type': 'std::string',
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
    'unicode_type': 'strings unsupported by numpy',
}


def is_item_type(type_):
    if str(type_) in NUMPY_DTYPE_MAP:
        return True
    if isinstance(type_, nb.types.Tuple):
        return all(map(is_item_type, type_.types))
    if isinstance(type_, nb.types.Array):
        return is_item_type(type_.dtype)
    if isinstance(type_, nb.types.Record):
        return all(map(lambda tp: is_item_type(tp[0]), type_.fields.values()))
    if isinstance(type_, nb.types.List):
        return is_item_type(type_.dtype)
    return False


def make_flat_tuple(type_):
    assert is_item_type(type_), "Expected valid nested tuple type."
    if str(type_) in NUMPY_DTYPE_MAP:
        return make_tuple([type_])
    if isinstance(type_, nb.types.Tuple):
        field_types = [make_flat_tuple(t) for t in flatten(type_.types)]
        return make_tuple([t.types[0] for t in field_types])
    if isinstance(type_, nb.types.Array):
        type_ = type_.copy(dtype=make_flat_tuple(type_.dtype))
        return make_tuple([replace_unituple(type_)])
    if isinstance(type_, nb.types.Record):
        fields = sorted(type_.dtype.fields.values(), key=lambda v: v[1])
        field_types = [nb.from_dtype(field[0]) for field in fields]
        return make_flat_tuple(make_tuple(field_types))
    if isinstance(type_, nb.types.List):
        type_ = type_.copy(dtype=make_flat_tuple(type_.dtype))
        return make_tuple([replace_unituple(type_)])
    assert False, "Any valid nested tuple type should be treated by now."
    return None


def make_record(type_, names):
    if str(type_) in NUMPY_DTYPE_MAP:
        types = [type_]
    elif isinstance(type_, nb.types.Tuple):
        types = type_.types
    elif isinstance(type_, nb.types.Record):
        fields = sorted(type_.dtype.fields.values(), key=lambda v: v[1])
        types = [nb.from_dtype(field[0]) for field in fields]
    else:
        assert False, "Cannot make record from {}".format(type_)
    names = list(names)
    if len(names) != len(types):
        raise ValueError(
            "Provided number of names does not match number of fields:\n"
            "    Names: {}\n"
            "    Tuple types: {}".format(names, types))
    dtypes = [NUMPY_DTYPE_MAP[str(t)] for t in types]
    record = np.dtype(list(zip(names, dtypes)))
    return nb.numpy_support.from_dtype(record)


def numba_type_to_dtype(type_):
    if str(type_) in NUMPY_DTYPE_MAP:
        return nb.numpy_support.as_dtype(type_)
    if isinstance(type_, nb.types.Tuple):
        child_types = [numba_type_to_dtype(t) for t in type_.types]
        fields = [('f%i' % i, t) for i, t in enumerate(child_types)]
        return np.dtype(fields)
    if isinstance(type_, nb.types.Array):
        return numba_type_to_dtype(type_.dtype)
    # for some reason record dtype is numpy dtype and arrays is not
    if isinstance(type_, nb.types.Record):
        return type_.dtype
    assert not is_item_type(type_)
    raise TypeError("Expected valid nested tuple type.")


def dtype_to_numba(type_):
    if type_.fields:
        # composite type
        types_ = []
        for _, val in type_.fields.items():
            numba_type = dtype_to_numba(val[0])
            types_.append(numba_type)
        return "(" + ",".join(types_) + ")"
    return nb.typeof(type_.name)


def get_type_size(type_):
    size = 0
    try:
        size = int(type_.bitwidth / 8)
    except AttributeError:
        if isinstance(type_, nb.types.Boolean):
            return 1
        if isinstance(type_, nb.types.BaseTuple):
            for child_type in type_.types:
                size += get_type_size(child_type)
        elif isinstance(type_, nb.types.Record):
            size = type_.size
        else:
            assert False, "Cannot compute size of " + type_.name
    return size


class RDDEncoder(JSONEncoder):
    # pylint: disable=method-hidden
    # sabir 14.02.18: JSONEncoder overwrites this method, nothing we can do
    def default(self, o):
        if isinstance(o, (nb.types.Number, nb.types.Boolean,
                          nb.types.misc.UnicodeType)):
            return {'type': numba_to_c_types(o.name)}
        if isinstance(o, nb.types.Tuple):
            return list(map(self.default, o.types))
        if isinstance(o, nb.types.Array):
            return {'type': 'array', 'num_dimensions': o.ndim,
                    'layout': o.layout, 'tuple_type': self.default(o.dtype)}
        if isinstance(o, nb.types.List):
            # treat this as an array of 1 dim
            return {'type': 'array', 'num_dimensions': 1, 'layout': 'C',
                    'output_type': self.default(o.dtype)}
        return JSONEncoder.default(self, o)


def error_print(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def item_typeof(expression):
    return replace_unituple(nb.typeof(expression))


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
    # This numba numpy type cannot contain numba Tuple type
    if isinstance(type_, nb.types.Record):
        return type_
    if isinstance(type_, nb.types.Array):
        type_.dtype = replace_unituple(type_.dtype)
        return type_
    if isinstance(type_, nb.types.IntegerLiteral):
        type_ = nb.types.Integer.from_bitwidth(type_.bitwidth, type_.signed)
    if str(type_) in NUMPY_DTYPE_MAP:
        return type_
    raise TypeError("Can only replace UniTuple on valid nested objects.")


def get_project_path():
    path = ""
    try:
        path = os.environ['JITQPATH']
    except KeyError:
        error_print(
            "JITQPATH is not defined, set it to your jitq installation path")
        sys.exit(1)
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
            elif isinstance(i, (nb.types.UniTuple, nb.types.Tuple,
                                nb.types.NamedTuple)):
                for j in rec(i.types):
                    yield j
            else:
                yield i

    if not isinstance(iterable_,
                      (tuple, list, nb.types.UniTuple, nb.types.Tuple,
                       nb.types.NamedTuple)):
        return (iterable_,)
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


NAMED_TUPLE_REGISTRY = {}


def replace_record(type_):
    if isinstance(type_, nb.types.BaseAnonymousTuple):
        child_types = [replace_record(t) for t in type_.types]
        return make_tuple(child_types)
    if isinstance(type_, nb.types.Array):
        dtype = replace_record(type_.dtype)
        return nb.types.Array(dtype, type_.ndim, type_.layout, False,
                              type_.name, type_.aligned)
    if isinstance(type_, nb.types.Record):
        fields_sorted = sorted(
            type_.dtype.fields.items(),
            key=lambda item: item[1][1])
        child_types = [replace_record(nb.from_dtype(v[1][0]))
                       for v in fields_sorted]
        child_names = [v[0] for v in fields_sorted]
        key = str([child_names] + [child_types])
        namedtpl = NAMED_TUPLE_REGISTRY.setdefault(
            key, namedtuple('Record', child_names))
        return nb.types.NamedTuple(child_types, namedtpl)
    if str(type_) in NUMPY_DTYPE_MAP:
        return type_
    raise TypeError("Can only replace UniTuple on valid nested objects.")
