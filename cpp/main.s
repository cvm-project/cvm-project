	.text
	.file	"main.cpp"
	.globl	_Z3addll
	.align	16, 0x90
	.type	_Z3addll,@function
_Z3addll:                               # @_Z3addll
	.cfi_startproc
# BB#0:
	leaq	(%rdi,%rsi), %rax
	retq
.Lfunc_end0:
	.size	_Z3addll, .Lfunc_end0-_Z3addll
	.cfi_endproc

	.globl	_Z3sumv
	.align	16, 0x90
	.type	_Z3sumv,@function
_Z3sumv:                                # @_Z3sumv
	.cfi_startproc
# BB#0:
	movabsq	$-8589934592, %rax      # imm = 0xFFFFFFFE00000000
	retq
.Lfunc_end1:
	.size	_Z3sumv, .Lfunc_end1-_Z3sumv
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	movabsq	$-8589934592, %rsi      # imm = 0xFFFFFFFE00000000
	movl	$_ZSt4cout, %edi
	callq	_ZNSo9_M_insertIlEERSoT_
	movl	$.L.str, %esi
	movl	$1, %edx
	movq	%rax, %rdi
	callq	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc

	.section	.text.startup,"ax",@progbits
	.align	16, 0x90
	.type	_GLOBAL__sub_I_main.cpp,@function
_GLOBAL__sub_I_main.cpp:                # @_GLOBAL__sub_I_main.cpp
	.cfi_startproc
# BB#0:
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movl	$_ZStL8__ioinit, %edi
	callq	_ZNSt8ios_base4InitC1Ev
	movl	$_ZNSt8ios_base4InitD1Ev, %edi
	movl	$_ZStL8__ioinit, %esi
	movl	$__dso_handle, %edx
	popq	%rax
	jmp	__cxa_atexit            # TAILCALL
.Lfunc_end3:
	.size	_GLOBAL__sub_I_main.cpp, .Lfunc_end3-_GLOBAL__sub_I_main.cpp
	.cfi_endproc

	.type	_ZStL8__ioinit,@object  # @_ZStL8__ioinit
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\n"
	.size	.L.str, 2

	.section	.init_array,"aw",@init_array
	.align	8
	.quad	_GLOBAL__sub_I_main.cpp

	.ident	"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"
	.section	".note.GNU-stack","",@progbits
