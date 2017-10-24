#include <dietfeatures.h>

#define SYS_exit 0x3c

.text

.weak exit
exit:
.global _exit
_exit:
	mov	$SYS_exit,%al

.global __error_unified_syscall
.global __unified_syscall
.global __unified_syscall_16bit
__unified_syscall:
	mov	$0, %ah
__unified_syscall_16bit:
	movzwl	%ax, %eax
#if defined(__ILP32__)
	or	$0x40000000, %eax
#endif
	mov	%rcx, %r10
	syscall
__error_unified_syscall:
	cmpq	$-132, %rax
	jbe	.Lnoerror
	negl	%eax
	pushq	%rax
	call	__errno_location
	popq	%rcx
	movl	%ecx,(%rax)
	orq	$-1, %rax
.Lnoerror:

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	ret
.Lhere:
	.size	 __unified_syscall,.Lhere-__unified_syscall
