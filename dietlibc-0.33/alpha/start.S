#include "dietfeatures.h"

.text
.align 2
	.set noreorder

.global __start
__start:
.global _start
_start:
	clr	$fp		/* clear frame pointer */

	br	$gp,.Lstart	/* set global pointer */
.Lstart:
	ldgp	$gp, 0($gp)

/* prepare to call main */
	ldl	$16, 0($sp)	/* argc / a0 */

	lda	$17, 8($sp)	/* argv / a1 */

	addq	$16,   1, $18	/* argp / a2 */
	s8addq	$18, $17, $18	/* (8*(argc+1))+argv -> argp  */

	stq	$18, environ

#ifdef WANT_DYNAMIC
/* in v0 ($0) is the ld.so _fini pointer */
	mov	 $0, $19	/* mov v0(dynload) to a3 */
	jsr	$26, _dyn_start
#else
	jsr	$26, CALL_IN_STARTCODE
#endif
	mov	 $0, $16
	jsr	$26, exit	/* YES, CALL! for threads and atexit ! (+4 byte) */
