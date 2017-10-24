#include <dietfeatures.h>
#include "syscalls.h"

.text
.weak exit
.type exit,function
exit:
.global _exit
.type	_exit,function
_exit:
	mov	__NR_exit, %g1
.global __unified_syscall
__unified_syscall:
	ta	0x10
	bcs	1f
	nop

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"
	retl
	nop

.global __unified_syscall_error_handler
__unified_syscall_error_handler:
1:	save	%sp, -96, %sp
#ifdef WANT_THREAD_SAFE
	call	__errno_location
	nop
#else
	sethi	%hi(errno), %o0
	or	%o0, %lo(errno), %o0
#endif
	st	%i0, [ %o0 ]
	ret
	restore %g0, -1, %o0

