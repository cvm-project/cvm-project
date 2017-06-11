import numba as nb


@nb.extending.intrinsic
def add(ctx, n1, n2):
    res_type = nb.types.int64
    sig = res_type(res_type, res_type)

    def codegen(context, builder, signature, args):
        [a, b] = args
        res = builder.add(a, b)
        return res

    return sig, codegen


@nb.njit()
def func(a, b):
    return add(a, b)


print(func(11, 17))
