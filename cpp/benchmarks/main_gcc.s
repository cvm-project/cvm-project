	.file	"main.cpp"
	.section	.text.unlikely,"ax",@progbits
.LCOLDB0:
	.text
.LHOTB0:
	.p2align 4,,15
	.globl	_Z3adddd
	.type	_Z3adddd, @function
_Z3adddd:
.LFB6514:
	.cfi_startproc
	vaddsd	%xmm1, %xmm0, %xmm0
	ret
	.cfi_endproc
.LFE6514:
	.size	_Z3adddd, .-_Z3adddd
	.section	.text.unlikely
.LCOLDE0:
	.text
.LHOTE0:
	.section	.text.unlikely
.LCOLDB4:
	.text
.LHOTB4:
	.p2align 4,,15
	.globl	_Z10simple_sumv
	.type	_Z10simple_sumv, @function
_Z10simple_sumv:
.LFB6515:
	.cfi_startproc
	vmovsd	.LC2(%rip), %xmm3
	vxorpd	%xmm1, %xmm1, %xmm1
	vmovapd	%xmm1, %xmm0
	vmovsd	.LC3(%rip), %xmm2
	.p2align 4,,10
	.p2align 3
.L3:
	vaddsd	%xmm1, %xmm0, %xmm0
	vaddsd	%xmm3, %xmm1, %xmm1
	vucomisd	%xmm1, %xmm2
	ja	.L3
	ret
	.cfi_endproc
.LFE6515:
	.size	_Z10simple_sumv, .-_Z10simple_sumv
	.section	.text.unlikely
.LCOLDE4:
	.text
.LHOTE4:
	.section	.text.unlikely
.LCOLDB8:
	.text
.LHOTB8:
	.p2align 4,,15
	.globl	_Z12unrolled_sumv
	.type	_Z12unrolled_sumv, @function
_Z12unrolled_sumv:
.LFB6516:
	.cfi_startproc
	vmovsd	.LC2(%rip), %xmm10
	vxorpd	%xmm2, %xmm2, %xmm2
	vmovapd	%xmm2, %xmm4
	vmovapd	%xmm2, %xmm3
	vmovsd	.LC5(%rip), %xmm9
	vmovapd	%xmm2, %xmm0
	vmovapd	%xmm2, %xmm1
	vmovsd	.LC6(%rip), %xmm8
	vmovsd	.LC7(%rip), %xmm7
	vmovsd	.LC3(%rip), %xmm6
	.p2align 4,,10
	.p2align 3
.L7:
	vaddsd	%xmm10, %xmm1, %xmm5
	vaddsd	%xmm1, %xmm2, %xmm2
	vaddsd	%xmm5, %xmm4, %xmm4
	vaddsd	%xmm9, %xmm1, %xmm5
	vaddsd	%xmm5, %xmm3, %xmm3
	vaddsd	%xmm8, %xmm1, %xmm5
	vaddsd	%xmm7, %xmm1, %xmm1
	vaddsd	%xmm5, %xmm0, %xmm0
	vucomisd	%xmm1, %xmm6
	ja	.L7
	vaddsd	%xmm4, %xmm2, %xmm2
	vaddsd	%xmm3, %xmm2, %xmm2
	vaddsd	%xmm0, %xmm2, %xmm0
	ret
	.cfi_endproc
.LFE6516:
	.size	_Z12unrolled_sumv, .-_Z12unrolled_sumv
	.section	.text.unlikely
.LCOLDE8:
	.text
.LHOTE8:
	.section	.text.unlikely
.LCOLDB9:
	.text
.LHOTB9:
	.p2align 4,,15
	.globl	_Z17map_filter_reducev
	.type	_Z17map_filter_reducev, @function
_Z17map_filter_reducev:
.LFB6517:
	.cfi_startproc
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	vxorpd	%xmm7, %xmm7, %xmm7
	vmovapd	%xmm7, %xmm2
	vmovsd	%xmm7, 24(%rsp)
	.p2align 4,,10
	.p2align 3
.L10:
	vaddsd	.LC2(%rip), %xmm2, %xmm3
	vmovapd	%xmm2, %xmm0
	vmovsd	%xmm2, 8(%rsp)
	vmovsd	.LC5(%rip), %xmm1
	vmovsd	%xmm3, 16(%rsp)
	call	fmod
	vmovsd	8(%rsp), %xmm2
	vucomisd	.LC2(%rip), %xmm0
	vmovsd	16(%rsp), %xmm3
	jp	.L15
	je	.L13
.L15:
	vaddsd	24(%rsp), %xmm2, %xmm5
	vmovsd	%xmm5, 24(%rsp)
.L13:
	vmovsd	.LC3(%rip), %xmm4
	vmovapd	%xmm3, %xmm2
	vucomisd	%xmm3, %xmm4
	ja	.L10
	vmovsd	24(%rsp), %xmm0
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE6517:
	.size	_Z17map_filter_reducev, .-_Z17map_filter_reducev
	.section	.text.unlikely
.LCOLDE9:
	.text
.LHOTE9:
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC10:
	.string	" simple sum\n"
.LC11:
	.string	" unrolled sum\n"
.LC12:
	.string	" vectorized sum2\n"
.LC13:
	.string	" map filter reduce\n"
	.section	.text.unlikely
.LCOLDB14:
	.section	.text.startup,"ax",@progbits
.LHOTB14:
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB6518:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	call	_ZNSt6chrono3_V212system_clock3nowEv
	vxorpd	%xmm0, %xmm0, %xmm0
	vmovsd	.LC2(%rip), %xmm2
	vmovsd	.LC3(%rip), %xmm1
	movq	%rax, %rbx
	.p2align 4,,10
	.p2align 3
.L20:
	vaddsd	%xmm2, %xmm0, %xmm0
	vucomisd	%xmm0, %xmm1
	ja	.L20
	vmovsd	%xmm1, 8(%rsp)
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movl	$_ZSt4cout, %edi
	movabsq	$4835703278458516699, %rdx
	subq	%rbx, %rax
	movq	%rax, %rcx
	imulq	%rdx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	movq	%rdx, %rsi
	subq	%rax, %rsi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC10, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	call	_ZNSt6chrono3_V212system_clock3nowEv
	vmovsd	8(%rsp), %xmm1
	vxorpd	%xmm0, %xmm0, %xmm0
	vmovsd	.LC7(%rip), %xmm2
	movq	%rax, %rbx
	.p2align 4,,10
	.p2align 3
.L21:
	vaddsd	%xmm2, %xmm0, %xmm0
	vucomisd	%xmm0, %xmm1
	ja	.L21
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movl	$_ZSt4cout, %edi
	subq	%rbx, %rax
	movabsq	$4835703278458516699, %rbx
	movq	%rax, %rcx
	imulq	%rbx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	subq	%rax, %rdx
	movq	%rdx, %rsi
	movq	%rdx, %rbp
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC11, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movq	%rbp, %rsi
	movl	$_ZSt4cout, %edi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC12, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movq	%rax, %rbp
	call	_Z17map_filter_reducev
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movl	$_ZSt4cout, %edi
	subq	%rbp, %rax
	movq	%rax, %rcx
	imulq	%rbx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	movq	%rdx, %rsi
	subq	%rax, %rsi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC13, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	addq	$24, %rsp
	.cfi_def_cfa_offset 24
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE6518:
	.size	main, .-main
	.section	.text.unlikely
.LCOLDE14:
	.section	.text.startup
.LHOTE14:
	.section	.text.unlikely
.LCOLDB15:
	.section	.text.startup
.LHOTB15:
	.p2align 4,,15
	.type	_GLOBAL__sub_I__Z3adddd, @function
_GLOBAL__sub_I__Z3adddd:
.LFB6712:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$_ZStL8__ioinit, %edi
	call	_ZNSt8ios_base4InitC1Ev
	movl	$__dso_handle, %edx
	movl	$_ZStL8__ioinit, %esi
	movl	$_ZNSt8ios_base4InitD1Ev, %edi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	__cxa_atexit
	.cfi_endproc
.LFE6712:
	.size	_GLOBAL__sub_I__Z3adddd, .-_GLOBAL__sub_I__Z3adddd
	.section	.text.unlikely
.LCOLDE15:
	.section	.text.startup
.LHOTE15:
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I__Z3adddd
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC2:
	.long	0
	.long	1072693248
	.align 8
.LC3:
	.long	0
	.long	1102053376
	.align 8
.LC5:
	.long	0
	.long	1073741824
	.align 8
.LC6:
	.long	0
	.long	1074266112
	.align 8
.LC7:
	.long	0
	.long	1074790400
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
