#include <errno.h>
#include "syscalls.h"

.text
.align 4
.weak clone
clone:
.type __clone,#function
.global __clone
__clone:
	save	%sp, -96, %sp

	tst	%i0		/* check for function pointer */
	be	.Lerror
	tst	%i1		/* check for stack pointer */
	be	.Lerror
	nop

	mov	%i1, %o1	/* child-stack */
	mov	%i2, %o0	/* clone-flags */
	mov	__NR_clone, %g1
	ta	0x10		/* syscall: clone */
	bcs	.Lerror

	tst	%o1
	bne	.Lstart		/* we are the child :) */
	nop
	ret
	restore %o0, %g0, %o0	/* return child pid */

.Lerror:
	call	__errno_location
	mov	EINVAL, %l0
	st	%l0, [%o0]
	ret
	restore	%g0, -1, %o0

.Lstart:
	call	%i0		/* call child-function */
	mov	%i3, %o0	/* put arg in the right place for the child */

	call	_exit		/* child returned */
	nop

