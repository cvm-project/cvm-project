from concurrent.futures import ThreadPoolExecutor, as_completed

import numpy as np

import numba as nb


@nb.extending.intrinsic
def atomic_xchg(context, ptr, cmp, val):
    if isinstance(ptr, nb.types.CPointer):
        valtype = ptr.dtype
        sig = valtype(ptr, valtype, valtype)

        def codegen(context, builder, signature, args):
            [ptr, cmpval, value] = args
            res = builder.cmpxchg(ptr, cmpval, value, ordering='monotonic')
            oldval, succ = nb.cgutils.unpack_tuple(builder, res)
            return oldval

        return sig, codegen


@nb.extending.intrinsic
def cast_as_intp_ptr(context, ptrval):
    ptrty = nb.types.CPointer(nb.intp)
    sig = ptrty(nb.intp)

    def codegen(context, builder, signature, args):
        [val] = args
        llrety = context.get_value_type(signature.return_type)
        return builder.inttoptr(val, llrety)
    return sig, codegen


@nb.njit("intp(intp[:])")
def try_lock(lock):
    iptr = cast_as_intp_ptr(lock[0:].ctypes.data)
    old = atomic_xchg(iptr, 0, 1)
    return old == 0


@nb.njit("void(intp[:])")
def unlock(lock):
    iptr = cast_as_intp_ptr(lock[0:].ctypes.data)
    old = atomic_xchg(iptr, 1, 0)
    assert old == 1


@nb.njit("(intp[:], float64[:])", nogil=True)
def lock_and_work(locks, arr):
    failtimes = 0
    for _ in range(10000):
        for i in range(locks.size):
            # get lock pointer
            lock_ptr = locks[i:]
            # try to lock and do some work
            if try_lock(lock_ptr):
                arr[i] += 1
                # unlock
                unlock(lock_ptr)
                break
        else:
            # count number of times it failed to do work
            failtimes += 1
    return failtimes


def main():
    locks = np.zeros(3, dtype=np.intp)
    values = np.zeros(3, dtype=np.float64)
    assert lock_and_work(locks, values) == 0

    with ThreadPoolExecutor(max_workers=4) as e:
        futures = [e.submit(lock_and_work, locks, values) for _ in range(8)]
        for fut in as_completed(futures):
            print('failed to lock {0} times'.format(fut.result()))

    print(values)


if __name__ == '__main__':
    main()