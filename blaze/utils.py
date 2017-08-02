import time
from json import JSONEncoder

import numba

import numba.types as nb_types

ACTION = 'action'
DAG = 'dag'
ID = 'id'
OP = 'op'
FUNC = 'func'
PREDS = 'predecessors'
FROM = 'from'
TO = 'to'
STEP = 'step'
VALUES = 'values'
OUTPUT_TYPE = 'output_type'
FILTER = 'filter'
MAP = 'map'
FLAT_MAP = 'flat_map'


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


NumpyTypeMap = {
    'float32': 'f4',
    'float64': 'f8',
    'int32': 'i4',
    'int64': 'i8',
    'boolean': 'b1',
    'bool_': 'b1',
}


def numba_type_to_dtype(type_):
    if isinstance(type_, nb_types.Tuple):
        types = []
        for t in type_.types:
            types += numba_type_to_dtype(t)
        return ",".join(types)
    else:
        return NumpyTypeMap[type_]


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
