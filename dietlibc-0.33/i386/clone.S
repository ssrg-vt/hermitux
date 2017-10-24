#include "syscalls.h"
#include <errno.h>
#include "PIC.h"

.text
.type clone,@function
.weak clone
clone:
.type __clone,@function
.global __clone
__clone:
	mov	$-EINVAL, %eax

	movl	4(%esp), %ecx	/* have non null thread_funcion */
	jecxz	.Lclone_error

	movl	8(%esp), %ecx	/* have non null child_stack pointer */
	jecxz	.Lclone_error

	/* put the parameter on thread stack */
	subl	$8, %ecx

	movl	16(%esp), %eax	/* arg */
	movl	%eax, 4(%ecx)

	movl	4(%esp), %eax	/* thread_func */
	movl	%eax, 0(%ecx)

	/* the syscall */
	pushl	%ebx
	movl	16(%esp), %ebx	/* flags */
	movl	$__NR_clone, %eax
	int	$0x80
	popl	%ebx

	testl	%eax, %eax
	jl	.Lclone_error
	jne	.Lreturn
.Lstart_thread:
	xorl	%ebp,%ebp
	call	*%ebx
	pushl	%eax
	call	_exit

.Lclone_error:
	negl	%eax
	pushl	%eax
#ifdef PIC
	pushl	%ebx
	PIC_INIT
	call	__errno_location@PLT
	popl	%ebx
#else
	call	__errno_location
#endif
	popl	%ecx
	movl	%ecx, (%eax)
	orl	$-1, %eax
.Lreturn:
	ret
