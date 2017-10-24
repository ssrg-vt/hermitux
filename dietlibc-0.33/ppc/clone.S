#include <dietfeatures.h>
#include "syscalls.h"
#include <errno.h>

.text
.weak clone
clone:
.global __clone
__clone:
	cmpwi	     4,0	/* check have non null child_stack pointer */
	cmpwi	cr1, 3,0	/* check have non null thread_funcion */
	cror	eq,4*cr1+eq,eq	/* now if eq is set one is or both are zero */
	beq	.Lclone_error

	stwu	 1,-32(1)	/* alloc some space on the stack */
	stmw	29, 16(1)	/* save r29,r30,r31 on stack */

	rlwinm	4,4,0,0,27	/* mask out lower 4 bits */

	/* move parameter to positions clone wants them */
	mr	29,3		/* r29 = r3 fn */
	mr	30,4		/* r30 = r4 stack */
	mr	31,6		/* r31 = r6 arg */
	mr	3, 5		/* r3  = r5 flags */

	li	0, __NR_clone	/* load syscall nr. */
	sc

	cmpwi	cr1,3,0		/* compare return of syscall with 0 */
	crandc	4*cr1+eq,4*cr1+eq,so
	bne	.Lclone_parent	/* return was non zero -> .Lclone_parent */

	/* we are the cloned process */
	mr	 1, 30		/* set stack pointer */
	mtctr	29		/* set count register to fn ? */
	mr	 3, 31		/* set argument */
	bctrl			/* branch trough count register and link */
	b	_exit		/* exit thread */

.Lclone_parent:
	lmw	29,16(1)	/* restore saved registers */
	addi	 1, 1,32	/* free stack */
	bnslr+			/* had cloned a thread so return to parent */
	b	error_unified_syscall

.Lclone_error:
	li	3, EINVAL
	b	error_unified_syscall
