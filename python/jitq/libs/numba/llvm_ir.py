import re
from llvmlite import ir
from llvmlite.ir import VoidType
from numba import sigutils
from numba.ccallback import CFunc

from jitq.utils import get_type_size, replace_unituple, flatten, replace_record

# types larger than this are classified as "MEMORY" in amd64 ABI
MINIMAL_TYPE_SIZE = 16  # bytes


def get_llvm_ir(sig, func, **options):
    sig = sigutils.normalize_signature(sig)

    args, return_type = sig
    # replace the unituple with tuple
    # abi compliance
    return_type = replace_record(replace_unituple(return_type))
    args = tuple([replace_record(replace_unituple(arg)) for arg in args])

    res = JITQCFunc(func, (args, return_type), {}, options=options)

    # abi compliance
    # if the return type is small enough we have to return a struct
    # otherwise a return pointer is used

    res.compile()
    code = res.inspect_llvm()

    # Extract just the code of the function
    # pylint: disable=no-member
    # sabir 14.02.18: somehow pylint does not understand S = DOTALL = ... in re
    code_group = re.search('define [^\n\r]* @"cfunc.*', code, re.DOTALL)
    code_string = code_group.group(0)

    # make the function name not unique
    code_string = re.sub("@.*\".*\"", "@cfuncnotuniquename", code_string)
    return code_string


class JITQCFunc(CFunc):
    """
    Numba Cfunc wrapper that produces JITQ compatible LLVM IR

    Adjustments:
    1) amd64 ABI, return values larger than MINIMAL_TYPE_SIZE
    must be written into memory location supplied by the caller
    in ret_ptr*
    2) Tuple arguments are always unnested
    3) Arrays are passed as a tuple
    {dtype ptr, int64 size_1, ..., int64 size_num_dim}
    where dtype is the item type and is always a tuple
    """

    def __init__(self, pyfunc, sig, locals_, options):
        super().__init__(pyfunc, sig, locals_, options)
        return_type = sig[1]
        self.is_small_return_type = get_type_size(
            return_type) <= MINIMAL_TYPE_SIZE

    def _compile_uncached(self):
        sig = self._sig

        # Compile native function
        cres = self._compiler.compile(sig.args, sig.return_type)
        assert not cres.objectmode  # disabled by compiler above
        fndesc = cres.fndesc

        # Compile C wrapper
        # Note we reuse the same library to allow inlining the Numba
        # function inside the wrapper.
        library = cres.library
        module = library.create_ir_module(fndesc.unique_name)
        context = cres.target_context

        ll_argtypes = [context.get_value_type(ty) for ty in flatten(sig.args)]
        if not self.is_small_return_type:
            resptr = context.call_conv.get_return_type(sig.return_type)

            ll_argtypes = [resptr] + ll_argtypes
            ll_return_type = VoidType()

            wrapty = ir.FunctionType(ll_return_type, ll_argtypes)
            wrapfn = module.add_function(wrapty,
                                         fndesc.llvm_cfunc_wrapper_name)
            builder = ir.IRBuilder(wrapfn.append_basic_block('entry'))

            # omit the first argument which is the res ptr
            self._build_c_wrapper_jitq(context, builder, cres, wrapfn.args[1:],
                                       wrapfn.args[0])
        else:
            ll_return_type = context.get_value_type(sig.return_type)

            wrapty = ir.FunctionType(ll_return_type, ll_argtypes)
            wrapfn = module.add_function(wrapty,
                                         fndesc.llvm_cfunc_wrapper_name)
            builder = ir.IRBuilder(wrapfn.append_basic_block('entry'))

            self._build_c_wrapper_jitq(context, builder, cres, wrapfn.args)

        library.add_ir_module(module)
        library.finalize()

        return cres

    def _build_c_wrapper_jitq(self, context, builder, cres, c_args,
                              retptr=None):
        sig = self._sig
        context.get_python_api(builder)

        fnty = context.call_conv.get_function_type(sig.return_type, sig.args)
        function_ir = builder.module.add_function(
            fnty, cres.fndesc.llvm_func_name)

        # sig.args and c_args should both be flatten
        sig.args = flatten(sig.args)
        _, out = context.call_conv.call_function(
            builder, function_ir, sig.return_type, sig.args, c_args)
        if not self.is_small_return_type:

            # the out must be written to the retptr
            builder.store(out, retptr)
            builder.ret_void()
        else:
            builder.ret(out)
