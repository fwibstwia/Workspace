	.file	"max.c"
	.section	.rodata
.LC0:
	.string	"%20.18f\n"
	.text
	.globl	cal
	.type	cal, @function
cal:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movss	%xmm0, -20(%rbp)
	movss	%xmm1, -24(%rbp)
	movss	%xmm2, -28(%rbp)
	movss	-20(%rbp), %xmm0
	movaps	%xmm0, %xmm1
	mulss	-20(%rbp), %xmm1
	movss	-24(%rbp), %xmm0
	movaps	%xmm0, %xmm2
	mulss	-24(%rbp), %xmm2
	movss	-28(%rbp), %xmm0
	mulss	-28(%rbp), %xmm0
	addss	%xmm2, %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	movss	-4(%rbp), %xmm0
	cvtps2pd	%xmm0, %xmm0
	movl	$.LC0, %edi
	movl	$1, %eax
	call	printf
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	cal, .-cal
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	.LC1(%rip), %eax
	movl	%eax, -12(%rbp)
	movl	.LC2(%rip), %eax
	movl	%eax, -8(%rbp)
	movl	.LC3(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %edx
	movl	-12(%rbp), %eax
	movl	%ecx, -20(%rbp)
	movss	-20(%rbp), %xmm2
	movl	%edx, -20(%rbp)
	movss	-20(%rbp), %xmm1
	movl	%eax, -20(%rbp)
	movss	-20(%rbp), %xmm0
	call	cal
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC1:
	.long	1084201336
	.align 4
.LC2:
	.long	1083380741
	.align 4
.LC3:
	.long	1092269679
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
