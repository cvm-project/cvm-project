import numba


from numba import njit, jit


def group_by_key(values):
    @jit()
    def inner(vals):
        grouped = []
        g_key = None
        g_counter = -1
        for key, val in vals:
            if key == g_key:
                grouped[g_counter][1].append(val)
            else:
                # create a new group
                grouped.append((key, [val]))
                g_key = key
                g_counter += 1
        return grouped

    # sort by key
    vals = sorted(values, key=lambda v: v[0])
    return iter(inner(vals))

@jit
def reduce_by_key(f, values):
    grouped = group_by_key(values)
    for k, vals in grouped:
        acc = reduce(f, vals)
        yield (k, acc)


def reduce(f, values):
    if not values:
        return 0
    acc = values[0]
    for v in values[1:]:
        acc = f(acc, v)
    return acc


def join(rel1, rel2):
    # hash
    res1 = {}
    for tupl in rel1:
        k = tupl[0]
        if k not in res1:
            res1[k] = []
        try:
            res1[k] += tupl[1]
        except TypeError:
            res1[k].append(tupl[1])

    res2 = {}
    for tupl in rel2:
        k = tupl[0]
        if k not in res2:  # log n
            res2[k] = []
        try:
            res2[k] += tupl[1]
        except TypeError:
            res2[k].append(tupl[1])

    res = {}
    for k, v in res1.items():
        if k in res2:
            res[k] = v + res2[k]
    return iter(res.items())


def map_(func, values):
    l_func = njit()(func)

    # @jit
    def inner():
        for v in values:
            yield l_func(v)

    res = inner()
    return res


def filter_(func, values):
    l_func = njit()(func)

    @njit
    def inner():
        for v in values:
            if l_func(v):
                yield v

    res = inner()
    return res
