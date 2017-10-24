#include <setjmp.h>

.text
.global __setjmp
.type	__setjmp,function
__setjmp:
	b	1f
	mov	0, %o1
.size __setjmp,.-__setjmp

.global setjmp
.type	setjmp,function
setjmp:
	mov	1, %o1
.size setjmp,.-setjmp

.global __sigsetjmp
.type	__sigsetjmp,function
__sigsetjmp:
1:
	st	%o1, [ %o0+FLAG_SAVEMASK ]	/* restore sigmask in longjmp ? */
	mov	%g0, %g1			/* return a 0 */
	ta	0x6e				/* getcontext / save context */
	retl
	mov	%g1, %o0			/* return value from longjmp or 0 */
.size __sigsetjmp,.-__sigsetjmp

