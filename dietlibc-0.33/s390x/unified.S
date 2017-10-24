#include <dietfeatures.h>
#include "syscalls.h"

.text
.weak exit
.type exit,@function
exit:
.global _exit
.type   _exit,@function
_exit:
	svc	__NR_exit
.global __unified_syscall
__unified_syscall:
	lghi	%r4,-127
	clr	%r2,%r4
	jl	.Lnoerror

.global __error_unified_syscall
__error_unified_syscall:
#ifdef WANT_THREAD_SAFE
	stmg	%r13,%r15,104(%r15)
	lgr	%r0,%r15
	aghi	%r15,-160
	stg	%r0,0(%r15)

	lcr	%r13,%r2
	brasl	%r14,__errno_location
	st	%r13,0(%r2)

	lg	%r15,0(%r15)
	lmg	%r13,%r15,104(%r15)
#else
	lcr	%r1,%r2
	larl	%r3,errno
	st	%r1,0(%r3)
#endif
	lghi	%r2,-1

.Lnoerror:

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	br	%r14

.align 4
