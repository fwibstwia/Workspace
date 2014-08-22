	.file	"max.c"
	.text
	.p2align 4,,15
	.globl	cal
	.type	cal, @function
cal:
.LFB16:
	.cfi_startproc
	vmulss	%xmm1, %xmm1, %xmm1
	vfmadd132ss	%xmm2, %xmm1, %xmm2
	vfmadd132ss	%xmm0, %xmm2, %xmm0
	ret
	.cfi_endproc
.LFE16:
	.size	cal, .-cal
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"%f\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB17:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$.LC1, %esi
	movl	$1, %edi
	vmovsd	.LC0(%rip), %xmm0
	movl	$1, %eax
	call	__printf_chk
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE17:
	.size	main, .-main
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC0:
	.long	536870912
	.long	1076279889
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
