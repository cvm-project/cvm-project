import re
from llvmlite import ir
from llvmlite.ir import VoidType
from numba import sigutils
from numba.ccallback import CFunc

from jitq.utils import replace_unituple, flatten, replace_record


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
    1) Return by pointer to packed struct
    2) Tuple arguments are always unnested
    3) Arrays are passed as a tuple
    {dtype ptr, int64 size_1, ..., int64 size_num_dim}
    where dtype is the item type and is always a tuple
    """

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

        # Compute LLVM types of fields input tuples
        ll_argtypes = [context.get_value_type(ty) for ty in flatten(sig.args)]
        ll_resptr = context.call_conv.get_return_type(sig.return_type)

        # If result is a struct, make it a packed one
        if isinstance(ll_resptr.pointee, ir.types.LiteralStructType):
            ll_restype = ir.LiteralStructType(ll_resptr.pointee, True)
            ll_resptr = ll_restype.as_pointer()

        # Create function signature and stub for wrapper
        ll_argtypes = [ll_resptr] + ll_argtypes
        ll_return_type = VoidType()

        wrapty = ir.FunctionType(ll_return_type, ll_argtypes)
        wrapfn = module.add_function(wrapty,
                                     fndesc.llvm_cfunc_wrapper_name)
        builder = ir.IRBuilder(wrapfn.append_basic_block('entry'))

        # Build the function body of the wrapper
        self._build_c_wrapper_jitq(context, builder, cres, wrapfn.args[1:],
                                   wrapfn.args[0])

        # Finalize and compile
        library.add_ir_module(module)
        library.finalize()

        return cres

    def _build_c_wrapper_jitq(self, context, builder, cres, args, retptr):
        # pylint: disable=too-many-locals  # Doesn't make sense to split...
        # Look up original function
        sig = self._sig
        context.get_python_api(builder)

        fnty = context.call_conv.get_function_type(sig.return_type, sig.args)
        function_ir = builder.module.add_function(
            fnty, cres.fndesc.llvm_func_name)

        # Call original function and store result in 'out'
        sig.args = flatten(sig.args)
        _, out = context.call_conv.call_function(
            builder, function_ir, sig.return_type, sig.args, args)

        # Copy result into packed struct
        ll_resty = retptr.type.pointee

        if isinstance(ll_resty, ir.types.LiteralStructType):
            # Copy struct element-wise to convert it to a packed one
            ll_tmpptr = builder.alloca(ll_resty)
            ll_res = builder.load(ll_tmpptr)
            for i in range(len(ll_resty)):
                val = builder.extract_value(out, i)
                ll_res = builder.insert_value(ll_res, val, i)
        else:
            # Copy everything else as is
            ll_res = out

        # Copy packed struct into result pointer
        cast_retptr = builder.bitcast(retptr, ir.PointerType(ll_res.type))
        builder.store(ll_res, cast_retptr)
        builder.ret_void()
