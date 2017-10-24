#include <dietfeatures.h>
#include "syscalls.h"

.weak exit
exit:
.global _exit
_exit:
	lda	$0, __NR_exit
.global __unified_syscall
__unified_syscall:
	callsys
	bne	$19, .Lerror

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	ret	$31, ($26), 0x01

.Lerror:
.global error_unified_syscall
error_unified_syscall:
#ifdef WANT_THREAD_SAFE
	lda	$sp, -16($sp)	/* alloc 2 qwords on stack */
	stq	$26, 0($sp)	/* save ra to stack */
	stq	$0,  8($sp)	/* save v0 to stack */

	jsr	$26, __errno_location	/* call __errno_location */

	ldq	$1,  8($sp)	/* write old v0 to errno */
	stl	$1,  0($0)

	lda	$0,  -1($31)	/* new return value is -1 */
	ldq	$26, 0($sp)	/* restore return address */
	lda	$sp, 16($sp)	/* free 2 qwords on stack */
#else
	lda	$1,  errno
	stl	$0,  0($1)
#endif
	ret	$31, ($26), 0x01  /* return */
