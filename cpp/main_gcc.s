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
	vxorpd	%xmm1, %xmm1, %xmm1
	vmovsd	.LC2(%rip), %xmm3
	vmovsd	.LC3(%rip), %xmm2
	vmovapd	%xmm1, %xmm0
	.p2align 4,,10
	.p2align 3
.L3:
	vaddsd	%xmm1, %xmm0, %xmm0
	vaddsd	%xmm3, %xmm1, %xmm1
	vucomisd	%xmm1, %xmm2
	ja	.L3
	rep ret
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
	vxorpd	%xmm2, %xmm2, %xmm2
	vmovsd	.LC2(%rip), %xmm10
	vmovsd	.LC5(%rip), %xmm9
	vmovapd	%xmm2, %xmm4
	vmovapd	%xmm2, %xmm0
	vmovapd	%xmm2, %xmm3
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
	vaddsd	%xmm5, %xmm0, %xmm0
	vaddsd	%xmm8, %xmm1, %xmm5
	vaddsd	%xmm7, %xmm1, %xmm1
	vaddsd	%xmm5, %xmm3, %xmm3
	vucomisd	%xmm1, %xmm6
	ja	.L7
	vaddsd	%xmm4, %xmm2, %xmm2
	vaddsd	%xmm0, %xmm2, %xmm2
	vaddsd	%xmm3, %xmm2, %xmm0
	ret
	.cfi_endproc
.LFE6516:
	.size	_Z12unrolled_sumv, .-_Z12unrolled_sumv
	.section	.text.unlikely
.LCOLDE8:
	.text
.LHOTE8:
	.section	.text.unlikely
.LCOLDB13:
	.text
.LHOTB13:
	.p2align 4,,15
	.globl	_Z14vectorized_sumv
	.type	_Z14vectorized_sumv, @function
_Z14vectorized_sumv:
.LFB6517:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	vxorpd	%xmm2, %xmm2, %xmm2
	pushq	-8(%r10)
	pushq	%rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	movq	%rsp, %rbp
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x78,0x6
	subq	$168, %rsp
	vmovsd	%xmm2, -160(%rbp)
	vmovsd	%xmm2, -152(%rbp)
	vmovsd	%xmm2, -144(%rbp)
	vmovsd	%xmm2, -136(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	vmovsd	.LC12(%rip), %xmm6
	vmovapd	-160(%rbp), %xmm4
	vinsertf128	$0x1, -144(%rbp), %ymm4, %ymm4
	vmovsd	.LC3(%rip), %xmm5
	.p2align 4,,10
	.p2align 3
.L10:
	vmovsd	.LC2(%rip), %xmm7
	movq	$0, -96(%rbp)
	vmovsd	.LC6(%rip), %xmm3
	vmovsd	%xmm7, -88(%rbp)
	vmovsd	.LC5(%rip), %xmm7
	vmovsd	%xmm2, -128(%rbp)
	vmovsd	%xmm2, -120(%rbp)
	vmovsd	%xmm7, -80(%rbp)
	vmovsd	%xmm3, -72(%rbp)
	vmovsd	.LC7(%rip), %xmm7
	vmovsd	.LC9(%rip), %xmm3
	vmovsd	%xmm2, -112(%rbp)
	vmovsd	%xmm2, -104(%rbp)
	vaddsd	%xmm6, %xmm2, %xmm2
	vmovsd	%xmm7, -64(%rbp)
	vmovsd	%xmm3, -56(%rbp)
	vmovsd	.LC10(%rip), %xmm1
	vmovsd	.LC11(%rip), %xmm7
	vmovapd	-128(%rbp), %xmm0
	vucomisd	%xmm2, %xmm5
	vinsertf128	$0x1, -112(%rbp), %ymm0, %ymm3
	vmovsd	%xmm1, -48(%rbp)
	vmovsd	%xmm7, -40(%rbp)
	vmovapd	-96(%rbp), %xmm1
	vmovapd	-64(%rbp), %xmm0
	vinsertf128	$0x1, -80(%rbp), %ymm1, %ymm1
	vinsertf128	$0x1, -48(%rbp), %ymm0, %ymm0
	vaddpd	%ymm3, %ymm1, %ymm1
	vaddpd	%ymm3, %ymm0, %ymm0
	vaddpd	%ymm0, %ymm1, %ymm0
	vaddpd	%ymm0, %ymm4, %ymm4
	ja	.L10
	vmovaps	%xmm4, -160(%rbp)
	vextractf128	$0x1, %ymm4, -144(%rbp)
	movq	-24(%rbp), %rax
	xorq	%fs:40, %rax
	vmovsd	-160(%rbp), %xmm0
	vaddsd	-152(%rbp), %xmm0, %xmm0
	vaddsd	-144(%rbp), %xmm0, %xmm0
	vaddsd	-136(%rbp), %xmm0, %xmm0
	jne	.L14
	vzeroupper
	addq	$168, %rsp
	popq	%r10
	.cfi_remember_state
	.cfi_def_cfa 10, 0
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
.L14:
	.cfi_restore_state
	vzeroupper
	call	__stack_chk_fail
	.cfi_endproc
.LFE6517:
	.size	_Z14vectorized_sumv, .-_Z14vectorized_sumv
	.section	.text.unlikely
.LCOLDE13:
	.text
.LHOTE13:
	.section	.text.unlikely
.LCOLDB15:
	.text
.LHOTB15:
	.p2align 4,,15
	.globl	_Z15vectorized_sum2v
	.type	_Z15vectorized_sum2v, @function
_Z15vectorized_sum2v:
.LFB6518:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	vxorpd	%xmm3, %xmm3, %xmm3
	pushq	-8(%r10)
	pushq	%rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	movq	%rsp, %rbp
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x78,0x6
	subq	$296, %rsp
	vmovsd	%xmm3, -288(%rbp)
	vmovsd	%xmm3, -280(%rbp)
	vmovsd	%xmm3, -272(%rbp)
	vmovsd	%xmm3, -264(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	vmovsd	.LC14(%rip), %xmm8
	vmovapd	-288(%rbp), %xmm5
	vinsertf128	$0x1, -272(%rbp), %ymm5, %ymm5
	vmovsd	.LC3(%rip), %xmm7
	vmovapd	%ymm5, %ymm6
	.p2align 4,,10
	.p2align 3
.L16:
	vmovsd	.LC2(%rip), %xmm2
	movq	$0, -224(%rbp)
	vmovsd	.LC7(%rip), %xmm4
	movq	$0, -160(%rbp)
	vmovsd	%xmm2, -216(%rbp)
	vmovsd	.LC5(%rip), %xmm2
	vmovsd	%xmm3, -256(%rbp)
	vmovsd	%xmm3, -248(%rbp)
	vmovsd	%xmm2, -208(%rbp)
	vmovsd	%xmm4, -192(%rbp)
	vmovsd	.LC6(%rip), %xmm2
	vmovsd	.LC10(%rip), %xmm4
	vmovsd	%xmm3, -240(%rbp)
	vmovsd	%xmm3, -232(%rbp)
	vaddsd	%xmm8, %xmm3, %xmm3
	vmovsd	%xmm2, -200(%rbp)
	vmovsd	%xmm4, -176(%rbp)
	vmovsd	.LC9(%rip), %xmm2
	vmovsd	.LC11(%rip), %xmm0
	vmovapd	-256(%rbp), %xmm4
	vucomisd	%xmm3, %xmm7
	vmovapd	-224(%rbp), %xmm1
	vinsertf128	$0x1, -240(%rbp), %ymm4, %ymm4
	vmovsd	%xmm2, -184(%rbp)
	vinsertf128	$0x1, -208(%rbp), %ymm1, %ymm1
	vmovsd	%xmm0, -168(%rbp)
	vmovsd	.LC5(%rip), %xmm0
	vmovsd	%xmm0, -144(%rbp)
	vaddpd	%ymm4, %ymm1, %ymm2
	vmovapd	-192(%rbp), %xmm1
	vinsertf128	$0x1, -176(%rbp), %ymm1, %ymm1
	vaddpd	%ymm4, %ymm1, %ymm1
	vaddpd	%ymm1, %ymm2, %ymm1
	vmovsd	.LC2(%rip), %xmm2
	vmovsd	%xmm2, -152(%rbp)
	vmovsd	.LC6(%rip), %xmm2
	vaddpd	%ymm1, %ymm5, %ymm5
	vmovsd	%xmm2, -136(%rbp)
	vmovsd	.LC7(%rip), %xmm0
	vmovsd	.LC9(%rip), %xmm2
	vmovsd	%xmm0, -96(%rbp)
	vmovsd	%xmm2, -88(%rbp)
	vmovsd	.LC10(%rip), %xmm0
	vmovsd	.LC11(%rip), %xmm2
	vmovsd	%xmm0, -80(%rbp)
	vmovsd	%xmm2, -72(%rbp)
	vmovapd	-160(%rbp), %xmm1
	vmovapd	-96(%rbp), %xmm0
	vinsertf128	$0x1, -144(%rbp), %ymm1, %ymm1
	vinsertf128	$0x1, -80(%rbp), %ymm0, %ymm0
	vaddpd	%ymm4, %ymm1, %ymm1
	vaddpd	%ymm4, %ymm0, %ymm0
	vaddpd	%ymm0, %ymm1, %ymm0
	vaddpd	%ymm0, %ymm6, %ymm6
	ja	.L16
	vmovaps	%xmm5, -160(%rbp)
	vextractf128	$0x1, %ymm5, -144(%rbp)
	vmovaps	%xmm6, -96(%rbp)
	vextractf128	$0x1, %ymm6, -80(%rbp)
	movq	-24(%rbp), %rax
	xorq	%fs:40, %rax
	vmovsd	-160(%rbp), %xmm0
	vaddsd	-152(%rbp), %xmm0, %xmm0
	vaddsd	-144(%rbp), %xmm0, %xmm0
	vaddsd	-136(%rbp), %xmm0, %xmm0
	vaddsd	-96(%rbp), %xmm0, %xmm0
	vaddsd	-88(%rbp), %xmm0, %xmm0
	vaddsd	-80(%rbp), %xmm0, %xmm0
	vaddsd	-72(%rbp), %xmm0, %xmm0
	jne	.L20
	vzeroupper
	addq	$296, %rsp
	popq	%r10
	.cfi_remember_state
	.cfi_def_cfa 10, 0
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
.L20:
	.cfi_restore_state
	vzeroupper
	call	__stack_chk_fail
	.cfi_endproc
.LFE6518:
	.size	_Z15vectorized_sum2v, .-_Z15vectorized_sum2v
	.section	.text.unlikely
.LCOLDE15:
	.text
.LHOTE15:
	.section	.text.unlikely
.LCOLDB16:
	.text
.LHOTB16:
	.p2align 4,,15
	.globl	_Z17map_filter_reducev
	.type	_Z17map_filter_reducev, @function
_Z17map_filter_reducev:
.LFB6519:
	.cfi_startproc
	vxorpd	%xmm7, %xmm7, %xmm7
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	vmovsd	%xmm7, 24(%rsp)
	vmovapd	%xmm7, %xmm2
	.p2align 4,,10
	.p2align 3
.L22:
	vaddsd	.LC2(%rip), %xmm2, %xmm3
	vmovapd	%xmm2, %xmm0
	vmovsd	.LC5(%rip), %xmm1
	vmovsd	%xmm2, 8(%rsp)
	vmovsd	%xmm3, 16(%rsp)
	call	fmod
	vucomisd	.LC2(%rip), %xmm0
	vmovsd	8(%rsp), %xmm2
	vmovsd	16(%rsp), %xmm3
	jp	.L27
	je	.L25
.L27:
	vaddsd	24(%rsp), %xmm2, %xmm5
	vmovsd	%xmm5, 24(%rsp)
.L25:
	vmovsd	.LC3(%rip), %xmm4
	vmovapd	%xmm3, %xmm2
	vucomisd	%xmm3, %xmm4
	ja	.L22
	vmovsd	24(%rsp), %xmm0
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE6519:
	.size	_Z17map_filter_reducev, .-_Z17map_filter_reducev
	.section	.text.unlikely
.LCOLDE16:
	.text
.LHOTE16:
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC17:
	.string	" simple sum\n"
.LC18:
	.string	" unrolled sum\n"
.LC19:
	.string	" vectorized sum\n"
.LC20:
	.string	" vectorized sum2\n"
.LC21:
	.string	" map filter reduce\n"
	.section	.text.unlikely
.LCOLDB22:
	.section	.text.startup,"ax",@progbits
.LHOTB22:
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB6520:
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
	movq	%rax, %rbx
	vmovsd	.LC2(%rip), %xmm2
	vmovsd	.LC3(%rip), %xmm1
	.p2align 4,,10
	.p2align 3
.L32:
	vaddsd	%xmm2, %xmm0, %xmm0
	vucomisd	%xmm0, %xmm1
	ja	.L32
	vmovsd	%xmm1, 8(%rsp)
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movabsq	$4835703278458516699, %rdx
	subq	%rbx, %rax
	movl	$_ZSt4cout, %edi
	movq	%rax, %rcx
	imulq	%rdx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	movq	%rdx, %rsi
	subq	%rax, %rsi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC17, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	call	_ZNSt6chrono3_V212system_clock3nowEv
	vxorpd	%xmm0, %xmm0, %xmm0
	movq	%rax, %rbx
	vmovsd	.LC7(%rip), %xmm2
	vmovsd	8(%rsp), %xmm1
	.p2align 4,,10
	.p2align 3
.L33:
	vaddsd	%xmm2, %xmm0, %xmm0
	vucomisd	%xmm0, %xmm1
	ja	.L33
	call	_ZNSt6chrono3_V212system_clock3nowEv
	subq	%rbx, %rax
	movabsq	$4835703278458516699, %rbx
	movl	$_ZSt4cout, %edi
	movq	%rax, %rcx
	imulq	%rbx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	subq	%rax, %rdx
	movq	%rdx, %rsi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC18, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movq	%rax, %rbp
	call	_Z14vectorized_sumv
	call	_ZNSt6chrono3_V212system_clock3nowEv
	subq	%rbp, %rax
	movl	$_ZSt4cout, %edi
	movq	%rax, %rcx
	imulq	%rbx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	movq	%rdx, %rsi
	subq	%rax, %rsi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC19, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movq	%rax, %rbp
	call	_Z15vectorized_sum2v
	call	_ZNSt6chrono3_V212system_clock3nowEv
	subq	%rbp, %rax
	movl	$_ZSt4cout, %edi
	movq	%rax, %rcx
	imulq	%rbx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	subq	%rax, %rdx
	movq	%rdx, %rsi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC20, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	call	_ZNSt6chrono3_V212system_clock3nowEv
	movq	%rax, %rbp
	call	_Z17map_filter_reducev
	call	_ZNSt6chrono3_V212system_clock3nowEv
	subq	%rbp, %rax
	movl	$_ZSt4cout, %edi
	movq	%rax, %rcx
	imulq	%rbx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$18, %rdx
	movq	%rdx, %rsi
	subq	%rax, %rsi
	call	_ZNSo9_M_insertIlEERSoT_
	movl	$.LC21, %esi
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
.LFE6520:
	.size	main, .-main
	.section	.text.unlikely
.LCOLDE22:
	.section	.text.startup
.LHOTE22:
	.section	.text.unlikely
.LCOLDB23:
	.section	.text.startup
.LHOTB23:
	.p2align 4,,15
	.type	_GLOBAL__sub_I__Z3adddd, @function
_GLOBAL__sub_I__Z3adddd:
.LFB6714:
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
.LFE6714:
	.size	_GLOBAL__sub_I__Z3adddd, .-_GLOBAL__sub_I__Z3adddd
	.section	.text.unlikely
.LCOLDE23:
	.section	.text.startup
.LHOTE23:
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
	.long	1099956224
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
	.align 8
.LC9:
	.long	0
	.long	1075052544
	.align 8
.LC10:
	.long	0
	.long	1075314688
	.align 8
.LC11:
	.long	0
	.long	1075576832
	.align 8
.LC12:
	.long	0
	.long	1075838976
	.align 8
.LC14:
	.long	0
	.long	1076887552
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
