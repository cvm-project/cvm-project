import os
import time
from json import JSONEncoder

import numba

import numba.types as nb_types
import sys

from numba import typeof


def mean(l):
    return sum(l) / len(l)


def timer(func, max_rep=3):
    res = []
    for r in range(0, max_rep):
        t1 = time.perf_counter()
        resu = func()
        t2 = time.perf_counter()
        res.append(t2 - t1)
        print(resu)
    return mean(res)


CTypeMap = {
    'float32': 'float',
    'float64': 'double',
    'int32': 'int',
    'int64': 'long',
    'boolean': 'bool',
    'bool_': 'bool',
}


def numba_to_c_types(numba_type):
    numba_type = str(numba_type)
    for k, v in CTypeMap.items():
        numba_type = numba_type.replace(k, v)
    return numba_type


numpyDtypeMap = {
    'float32': 'f4',
    'float64': 'f8',
    'int32': 'i4',
    'int64': 'i8',
    'boolean': 'b1',
    'bool_': 'b1',
}


# dtypeNumbaMap = {
#     'f4': 'float32',
#     'f8': 'float64',
#     'i4': 'int32',
#     'i8': 'int64',
#     'b1': 'boolean',
# }

def numba_type_to_dtype(type_):
    if isinstance(type_, nb_types.Tuple):
        types = []
        for t in type_.types:
            w = numba_type_to_dtype(t)
            types.append(w)
        return ",".join(types)
    else:
        return numpyDtypeMap[type_.name]


def dtype_to_numba(type_):
    if type_.fields:
        # composite type
        types = []
        for _, v in type_.fields.items():
            w = dtype_to_numba(v[0])
            types.append(w)
        return "(" + ",".join(types) + ")"
    else:
        return typeof(type_.name)
        # return type_.name


def get_type_size(type_):
    size = 0
    try:
        size = int(type_.bitwidth / 8)
    except AttributeError:
        if isinstance(type_, nb_types.Tuple):
            for child_type in type_.types:
                size += get_type_size(child_type)
        elif isinstance(type_, nb_types.UniTuple):
            size = get_type_size(type_.dtype) * type_.count
    return size


class RDDEncoder(JSONEncoder):
    def default(self, o):
        if isinstance(o, numba.types.Type):
            return numba_to_c_types(o.name)
        return JSONEncoder.default(self, o)


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def make_tuple(child_types):
    out = nb_types.Tuple([])
    out.types = tuple(child_types)
    out.name = "(%s)" % ', '.join(str(i) for i in child_types)
    out.count = len(child_types)
    return out


def replace_unituple(type_):
    out = type_
    if isinstance(type_, nb_types.Tuple):
        child_types = []
        for child_type in type_.types:
            child_types.append(replace_unituple(child_type))
        out = nb_types.Tuple([])
        out.types = tuple(child_types)
        out.name = "(%s)" % ', '.join(str(i) for i in child_types)
        out.count = len(child_types)
    elif isinstance(type_, nb_types.UniTuple):
        type_type = nb_types.Tuple([])
        type_type.types = (replace_unituple(type_.dtype),) * type_.count
        type_type.count = type_.count
        type_type.name = "(%s)" % ', '.join(str(i) for i in type_type.types)
        out = type_type
    elif isinstance(type_, tuple):
        out = tuple(map(lambda t: replace_unituple(t), type_))
    elif isinstance(type_, list):
        out = list(map(lambda t: replace_unituple(t), type_))
    return out


def getBlazePath():
    blaze_path = ""
    try:
        blaze_path = os.environ['BLAZEPATH']
    except KeyError:
        eprint("BLAZEPATH is not defined, set it to your blaze installation path")
        exit(1)
    return blaze_path
