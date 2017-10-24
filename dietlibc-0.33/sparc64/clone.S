#include <errno.h>
#include "syscalls.h"

.text
.align 4
.weak clone
clone:
.type __clone,#function
.global __clone
__clone:
	save	%sp, -192, %sp
	brz,pn	%i0, .LerrorV	/* check for function pointer */
	mov	%i0, %l0
	brz,pn	%i1, .LerrorV	/* check for stack pointer */
	mov	%i3, %l3

	sub	%i1, 2047, %o1	/* child-stack + BIAS */
	mov	%i2, %o0	/* clone-flags */
	mov	__NR_clone, %g1
	ta	0x6d		/* syscall: clone */
	bcs,pn	%xcc, .Lerror
	nop
	brnz,pn	%o1, .Lstart	/* we are the child :) */
	mov	%o0, %i0	/* return child pid */
	ret
	restore

.LerrorV:
	mov	EINVAL, %i0
.Lerror:
	call	__errno_location
	nop
	st	%i0, [%o0]
	ret
	restore	%g0, -1, %o0

.Lstart:
	mov	%g0, %fp	/* close frame */
	sub	%sp,(6*8), %sp	/* make a little space */
	call	%l0		/* call child-function */
	mov	%l3, %o0	/* put arg in the right place for the child */
	call	_exit		/* child returned */
	nop

