
#include <dietfeatures.h>

	.text
#ifdef __ARM_EABI__
	.align 4
#else
	.align 2
#endif 	
	.global __unified_syscall
	.global __unified_syscall4


#ifdef __ARM_EABI__

__unified_syscall4:
__unified_syscall:
	
        cmn     r0, #4096
        rsbcs   r2, r0, #0
        ldrcs   r3, 1f
        mvncs   r0, #0
        strcs   r2, [r3]
        ldmfd   sp!,{r4,r5,r7,pc}
	.balign 4
1:
        .word   errno
	
/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	mov	pc, lr			@ return

#else	
	
__unified_syscall4:
	ldmia	sp!, {r4, r5, r6}
__unified_syscall:
	cmn	r0, #4096
	movcc	pc, lr			@ return value comes direct from kernel.

	rsb	r1, r0, #0
#ifdef WANT_THREAD_SAFE
	stmdb	sp!, {r1, lr}
	bl	__errno_location
	ldmia	sp!, {r1, lr}
#else
	ldr	r0, .L0
#endif
	str	r1, [r0]		@ store error code into 'errno'
	mvn	r0, #0			@ return value = -1

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	mov	pc, lr			@ return

#ifndef WANT_THREAD_SAFE
.L0:	.long 	errno
#endif

#endif
