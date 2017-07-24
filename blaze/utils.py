import time


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


typeMap = {
    'float32': 'float',
    'float64': 'double',
    'int32': 'int',
    'int64': 'long',
    'boolean': 'bool',
    'bool_': 'bool',
}


def numba_to_c_types(numba_type):
    numba_type = str(numba_type)
    for k, v in typeMap.items():
        numba_type = numba_type.replace(k, v)
    return numba_type
