	.file	"param.c"
	.text
	.globl	add
	.type	add, @function
add:
	subl	$4, %esp
	movl	8(%esp), %edx
	movl	12(%esp), %eax
	addl	%edx, %eax
	movl	%eax, (%esp)
	movl	(%esp), %eax
	addl	$4, %esp
	ret
	.size	add, .-add
	.globl	main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	%gs:20, %eax
	movl	%eax, -4(%ebp)
	xorl	%eax, %eax
	movl	$4, %eax
	subl	$1, %eax
	addl	$32, %eax
	movl	$4, %ecx
	movl	$0, %edx
	divl	%ecx
	sall	$2, %eax
	movl	%eax, %ecx
	andl	$-4096, %ecx
	movl	%esp, %edx
	subl	%ecx, %edx
.L4:
	cmpl	%edx, %esp
	je	.L5
	subl	$4096, %esp
	orl	$0, 4092(%esp)
	jmp	.L4
.L5:
	movl	%eax, %edx
	andl	$4095, %edx
	subl	%edx, %esp
	movl	%eax, %edx
	andl	$4095, %edx
	testl	%edx, %edx
	je	.L6
	andl	$4095, %eax
	subl	$4, %eax
	addl	%esp, %eax
	orl	$0, (%eax)
.L6:
	movl	%esp, %eax
	addl	$15, %eax
	shrl	$4, %eax
	sall	$4, %eax
	movl	%eax, -8(%ebp)
	movl	$0, %eax
	movl	-4(%ebp), %edx
	subl	%gs:20, %edx
	je	.L8
	call	__stack_chk_fail
.L8:
	leave
	ret
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
