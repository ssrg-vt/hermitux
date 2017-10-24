#include <setjmp.h>

.text
.weak	siglongjmp
siglongjmp:
.global __siglongjmp
__siglongjmp:
.weak	longjmp
longjmp:
.global __libc_longjmp
.type	__libc_longjmp,function
__libc_longjmp:
	movrz	%o1, 1, %o1			/* never return 0 in setjmp */
	stx	%o1, [ %o0 + (4+MC_G1) * 8 ]    /* save return value in context */
	ld	[ %o0+FLAG_SAVEMASK ], %o1	/* has setjmp saved the signalmask ? */
	ta	0x6f				/* setcontext / restore context */
.size	__libc_longjmp,.-__libc_longjmp
