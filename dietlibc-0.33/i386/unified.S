
#include <dietfeatures.h>

#ifdef WANT_SYSENTER
.data
.type __vsyscall,@object
.global __vsyscall
__vsyscall:
.Lvsyscall:
.long	.Lcallint80
#endif

.text
.global __unified_syscall_256
.type __unified_syscall_256,@function
__unified_syscall_256:
	movzwl %ax,%eax
	jmp .L1
.L2:
.size __unified_syscall_256,.L2-__unified_syscall_256
.weak exit
exit:
.global _exit
.type	_exit,@function
_exit:
	movb	$1,%al
.global __unified_syscall
.type	__unified_syscall,@function
__unified_syscall:
.size _exit,__unified_syscall-_exit
	movzbl	%al, %eax
.L1:
	push	%edi
	push	%esi
	push	%ebx
	push	%ebp
	movl	%esp,%edi
	/* we use movl instead of pop because otherwise a signal would
	   destroy the stack frame and crash the program, although it
	   would save a few bytes. */
	movl	5*4(%edi),%ebx
	movl	6*4(%edi),%ecx
	movl	7*4(%edi),%edx
	movl	8*4(%edi),%esi
	movl	10*4(%edi),%ebp
	movl	9*4(%edi),%edi
#ifdef WANT_SYSENTER
	call	*.Lvsyscall /* 0xffffe000 */
#else
	int	$0x80
#endif
	pop	%ebp
	pop	%ebx
	pop	%esi
	pop	%edi
	cmp	$-132,%eax
	jb	.Lnoerror
	neg	%eax
.global __error_unified_syscall
.type	__error_unified_syscall,@function
__error_unified_syscall:
#ifdef WANT_THREAD_SAFE
	push	%eax
	call	__errno_location
	pop	(%eax)
	orl	$-1,%eax
#else
	mov	%eax,errno
	sbb	%eax,%eax               # eax = eax - eax - CY = -1
#endif
.Lnoerror:

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	ret
.L3:
.size __unified_syscall,.L3-__unified_syscall
#ifdef WANT_SYSENTER
.Lcallint80:
	int	$0x80
	ret
#endif
