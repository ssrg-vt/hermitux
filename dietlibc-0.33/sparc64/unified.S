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
	ta	0x6d

	bcs	%xcc, 1f	/* error ? */
	nop

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"
	retl
	nop

.global __unified_syscall_error_handler
__unified_syscall_error_handler:
1:	save	%sp, -192, %sp
#ifdef WANT_THREAD_SAFE
	call	__errno_location
	nop
#else
	sethi	%hi(errno), %o0
	or	%o0, %lo(errno), %o0
#endif
	st	%i0, [ %o0 ]	/* store errno */
	ret
	restore %g0, -1, %o0	/* return -1 */

