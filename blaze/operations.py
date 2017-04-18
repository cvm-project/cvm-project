import numba
# from numba import njit, jit


def group_by_key(values):
    # @jit()
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
    return inner(vals)


def reduce_by_key(f, values):
    grouped = group_by_key(values)
    reduced = []
    for k, vals in grouped:
        acc = reduce(f, vals)
        reduced.append((k, acc))
    return reduced


def reduce(f, values):
    if not values:
        return 0
    acc = values[0]
    for v in values[1:]:
        acc = f(acc, v)
    return acc


def join(rel1, rel2):
    res = []
    r1 = group_by_key(rel1)
    r2 = group_by_key(rel2)
    ind2 = 0
    ind1 = 0
    while True:
        k1 = r1.next[0]
        k2 = r2.next[0]
        if k1 == k2:
            t = (k1, [])
            t[1].append(r1[ind1][1])
            t[1].append(r2[ind2][1])
            res.append(t)
        elif k1 < k2:
            ind1 += 1
        else:
            ind2 += 1
    return res


# inner will be recompiled on every call
def map_(func, values):
    l_func = func

    # @jit
    def inner(values):
        for v in values:
            yield l_func(v)

    res = inner(values)
    return res


def filter_(func, values):
    l_func = func

    # @jit
    def inner(values):
        for v in values:
            if (l_func(v)):
                yield v

    res = inner(values)
    return res
