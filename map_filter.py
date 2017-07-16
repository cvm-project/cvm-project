import os
from blaze.blaze_context import *


# sys.path.insert(0, "/home/sabir/projects/")

input = [(1,2,4)]
data = collection(input)

# os.environ["NUMBA_DUMP_IR"] = '1'
# os.environ["NUMBA_DUMP_ASSEMBLY"] = '1'
os.environ["NUMBA_DUMP_OPTIMIZED"] = '1'


# os.environ["NUMBA_DUMP_LLVM"] = '1'

def func(retptr, a, b, c):
    # for i in t:
    #     if i < 0:
    #         return False
    # return True
    retptr[0][0] = a


# func = lambda t: t[3]
w = data.filter(func)
w2 = data
w3 = w.join(w2)

# w = data.map(lambda w: (w, w * 2)).filter(lambda t: t[0] % 2 == 0 and t[1] % 3 == 0)
w.count()
# print(w.count())



#
# f(1, 2)
# with open('/home/sabir/add.ll', 'w') as file:
#     llvm = f.inspect_llvm()
#     for k, v in llvm.items():
#         file.write(str(k))
#         file.write(v)
#         file.write('\n')
#
# @jit()
# def f(a, adict):
#     return a
#
# print(f(2, {1:2}))
# @cfunc("int64, float64(float64, float64)")
# @njit()
# def callee(t):
#     print(t)
#     return 17
#
#
# @njit()
# def caller(a):
#     return a + 338 + callee(a)
#
#
# caller(1)
# # # f([1, 2, 3])
# # # # print(f(2.0))
# # # # print(f(1 << 700))
# # # # f([1.3], 2.3)
# # # # f((2, 4, 3))
# # # # print(f.inspect_types())
# f_llvm = f.inspect_llvm()
# for k, v in f_llvm.items():
#     print(str(v))
# mod = llvm.parse_assembly(v)
#     funcs = mod.functions
#     for fuk in funcs:
#         print(fuk)
