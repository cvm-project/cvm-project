
def group_by_key(values):
    # sort by key
    vals = sorted(values, key=lambda v: v[0])
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
    # hash
    res = {}
    for tupl in rel1:
        k = tupl[0]
        if k not in res:
            res[k] = []
        try:
            res[k] += tupl[1]
        except TypeError:
            res[k].append(tupl[1])

    for tupl in rel2:
        k = tupl[0]
        if k not in res:  # log n
            res[k] = []
        try:
            res[k] += tupl[1]
        except TypeError:
            res[k].append(tupl[1])

    return res.items()
