#include <dietfeatures.h>
#include "syscalls.h"

.text
.type exit,@function
.weak exit
exit:
.type _exit,@function
.global _exit
_exit:
	li	0,__NR_exit
.global __unified_syscall
__unified_syscall:
	sc
	bnslr+

.global error_unified_syscall
error_unified_syscall:
#ifdef WANT_THREAD_SAFE
	stwu	1,-16(1)
	mflr	0
	stw	0,20(1)
	stw	3,12(1)
	bl	__errno_location
	lwz	0,12(1)
	stw	0,0(3)
	lwz	0,20(1)
	mtlr	0
	addi	1,1,16
#else
	lis	9,errno@ha
	stw	3,errno@l(9)
#endif
	li	3,-1

/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	blr

