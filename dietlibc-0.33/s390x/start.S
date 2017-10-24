#include "dietfeatures.h"

.text
.global _start
_start:
#ifdef WANT_DYNAMIC
	lgr	%r5,%r14		/* save dynamic ld.so _fini */
#endif
	lg	%r2,0(%r15)		/* argc */
	la	%r3,8(%r15)		/* argv */

/* envp = argv + ( (argc+1)*8 ) */
	lgr	%r4,%r2
	sll	%r4,3
	la	%r4,16(%r4,%r15)

/* align the stack to a double word boundary */
	lghi	%r0,-16
	ngr	%r15,%r0

/* Setup a stack frame and a parameter area */
	aghi	%r15,-176               /* make room on stack */
	xc	0(8,%r15),0(%r15)       /* clear back-chain */

/* environ = envp */
	larl	%r13,environ
	stg	%r4,0(%r13)

/* call main or _dyn_start */
#ifdef WANT_DYNAMIC
	brasl	%r14,_dyn_start
#else
	brasl	%r14,CALL_IN_STARTCODE
#endif

/* call exit */
	brasl	%r14,exit

/* DIE! */
        .word   0

.align 4
.Lstart:
        .size   _start,.Lstart-_start
