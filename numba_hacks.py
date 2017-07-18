from blaze.libs.numba.llvm_ir import cfunc
from numba import njit, types
import os


# os.environ["NUMBA_DUMP_IR"] = '1'
os.environ["NUMBA_DUMP_OPTIMIZED"] = '1'
# os.environ["NUMBA_DUMP_LLVM"] = '1'
@cfunc(types.Tuple([types.intc, types.intc])(types.Tuple([types.intc, types.intc])))
def caller(a):
    return a


caller(3)