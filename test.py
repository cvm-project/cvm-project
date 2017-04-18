from numba import njit, jit


def w(z):
    return z

@jit
def f():
    sorted(range(0,10), key=w)

f()
print(f.inspect_asm())