#include <errno.h>
#include "syscalls.h"

.global __clone
__clone:
	comibt,= 0,%r26,.Lerror		/* function pointer ? */
	ldi	-EINVAL,%r28
	comibt,=,n 0,%r25,.Lerror	/* stack pointer ? */

	stwm	%r26,64(%sr0,%r25)	/* save function ptr	on thread stack */
	stw	%r23, 8(%sr0,%r25)	/* save argument	on thread stack */

	/* syscall */
	copy	%r24,%r26
	ble	0x100(%sr2,%r0)
	ldi	__NR_clone,%r20

	/* error ? */
	ldi	-4096,%r1
	combt,<<,n %r1,%r28,.Lerror

	/* am I the child ? */
	comibt,=,n 0,%r28,.Lthread_start

	/* parent return */
	bv,n	%r0(%r2)

.Lerror: /* there was an error / for nullification a forward reference */
	b	__error_unified_syscall
	sub	%r0,%r28,%r28		/* return=-return / kernel error to errno */

.Lthread_start:
	ldw	-64(%sr0,%r30),%r22	/* function ptr */
	ldw	8(%sr0,%r30),%r26	/* load argument */

	bl,n	$$dyncall,%r2		/* microcode call / is in libgcc */
	bl	_exit,%r2		/* exit thread */
	copy	%r28,%r26		/* return -> arg0 */

/*	iitlbp	%r0,(%r0) */		/* DIE ! DIE ! */
