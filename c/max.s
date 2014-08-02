	.file	"max.c"
	.section	.rodata
.LC9:
	.string	"%f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	.LC0(%rip), %eax
	movl	%eax, -40(%rbp)
	movl	.LC1(%rip), %eax
	movl	%eax, -36(%rbp)
	movl	.LC2(%rip), %eax
	movl	%eax, -32(%rbp)
	movl	.LC3(%rip), %eax
	movl	%eax, -28(%rbp)
	movl	.LC4(%rip), %eax
	movl	%eax, -24(%rbp)
	movl	.LC5(%rip), %eax
	movl	%eax, -20(%rbp)
	movss	-40(%rbp), %xmm0
	movaps	%xmm0, %xmm1
	mulss	-40(%rbp), %xmm1
	movss	-36(%rbp), %xmm0
	movaps	%xmm0, %xmm2
	mulss	-36(%rbp), %xmm2
	movss	-32(%rbp), %xmm0
	mulss	-32(%rbp), %xmm0
	addss	%xmm2, %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -16(%rbp)
	movss	-28(%rbp), %xmm0
	movaps	%xmm0, %xmm1
	mulss	-40(%rbp), %xmm1
	movss	-24(%rbp), %xmm0
	movaps	%xmm0, %xmm2
	mulss	-36(%rbp), %xmm2
	movss	-20(%rbp), %xmm0
	mulss	-32(%rbp), %xmm0
	addss	%xmm2, %xmm0
	addss	%xmm1, %xmm0
	movss	.LC6(%rip), %xmm1
	mulss	%xmm1, %xmm0
	movss	%xmm0, -12(%rbp)
	movss	-28(%rbp), %xmm0
	movaps	%xmm0, %xmm1
	mulss	-28(%rbp), %xmm1
	movss	-24(%rbp), %xmm0
	mulss	-24(%rbp), %xmm0
	addss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	mulss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	unpcklps	%xmm0, %xmm0
	cvtps2pd	%xmm0, %xmm0
	movsd	.LC7(%rip), %xmm1
	subsd	%xmm1, %xmm0
	unpcklpd	%xmm0, %xmm0
	cvtpd2ps	%xmm0, %xmm3
	movss	%xmm3, -8(%rbp)
	movss	-12(%rbp), %xmm0
	mulss	-12(%rbp), %xmm0
	movss	-16(%rbp), %xmm2
	movss	.LC8(%rip), %xmm1
	mulss	%xmm2, %xmm1
	mulss	-8(%rbp), %xmm1
	subss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	movss	-28(%rbp), %xmm0
	movaps	%xmm0, %xmm1
	mulss	-40(%rbp), %xmm1
	movss	-24(%rbp), %xmm0
	movaps	%xmm0, %xmm2
	mulss	-36(%rbp), %xmm2
	movss	-20(%rbp), %xmm0
	mulss	-32(%rbp), %xmm0
	addss	%xmm2, %xmm0
	addss	%xmm1, %xmm0
	unpcklps	%xmm0, %xmm0
	cvtps2pd	%xmm0, %xmm0
	movl	$.LC9, %edi
	movl	$1, %eax
	call	printf
	movss	-4(%rbp), %xmm0
	cvtps2pd	%xmm0, %xmm0
	movl	$.LC9, %edi
	movl	$1, %eax
	call	printf
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC0:
	.long	1040187392
	.align 4
.LC1:
	.long	3213214856
	.align 4
.LC2:
	.long	1078785552
	.align 4
.LC3:
	.long	3218164048
	.align 4
.LC4:
	.long	1083876662
	.align 4
.LC5:
	.long	3228067160
	.align 4
.LC6:
	.long	3221225472
	.align 8
.LC7:
	.long	3839494604
	.long	1076642288
	.align 4
.LC8:
	.long	1082130432
	.ident	"GCC: (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1"
	.section	.note.GNU-stack,"",@progbits
