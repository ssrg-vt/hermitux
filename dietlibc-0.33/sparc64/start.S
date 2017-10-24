#include "dietfeatures.h"

	.section ".text"
	.align 4
	.global _start
	.type _start,@function
_start:

#define BIAS 2047	/* for 64bit-register-window marking */

/* Terminate the stack frame, and reserve space for functions to
   drop their arguments.  */
	mov	%g0, %fp
	sub	%sp, 6*8, %sp

/* Extract the arguments and environment as encoded on the stack.  The
   argument info starts after one register window (16 words) past the SP.  */
	ldx	[%sp+(BIAS+(22*8))], %o0
	add	%sp, (BIAS+(23*8)), %o1
	smul	%o0, 8, %o2
	add	%o2, %o1, %o2
	add	%o2, 8, %o2

	sethi	%hi(environ), %o3
	or	%o3, %lo(environ), %o3
	stx	%o2, [%o3]

/* When starting a binary via the dynamic linker, %g1 contains the
   address of the shared library termination function, which will be
   registered with atexit(). If we are statically linked, this will
   be NULL.  */

/* Let libc do the rest of the initialization, and call main.  */
#ifdef WANT_DYNAMIC
	call	_dyn_start
#else
	call	CALL_IN_STARTCODE
#endif
	mov	%g1, %o3

	b	exit
	mov %o0, %i0

/* die in dishonor */
	ret
	nop

	.size _start, .-_start

