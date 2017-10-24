#include <dietfeatures.h>
#include "syscalls.h"

	.text
.exit:
	.weak	.exit
._exit:
	li	0,__NR_exit
	.global __unified_syscall
__unified_syscall:
	sc
	bnslr+

	.global error_unified_syscall
error_unified_syscall:

#ifdef WANT_THREAD_SAFE
	stdu	1,-128(1)
	mflr	0
	std	0,20(1)
	std	3,12(1)
	bl	.__errno_location
	ld	0,12(1)
	stw	0,0(3)
	ld	0,20(1)
	mtlr	0
	addi	1,1,128
#else
	lis	9,errno@highesta
	ori	9,9,errno@highera
	sldi    9,9,32
	oris	9,9,errno@ha
	stw	3,errno@l(9)
#endif
	li	3,-1

/* here we go and "reuse" the return for weak-void functions */
//#include "dietuglyweaks.h"
.__thread_doexit:
	.weak	.__thread_doexit
.__fflush_stdin:
	.weak	.__fflush_stdin
.__fflush_stdout:
	.weak	.__fflush_stdout
.__fflush_stderr:
	.weak	.__fflush_stderr
.flockfile:
	.weak	.flockfile
.funlockfile:
	.weak	.funlockfile
.__nop:
	.weak	.__nop
.__you_tried_to_link_a_dietlibc_object_against_glibc:
	.weak	.__you_tried_to_link_a_dietlibc_object_against_glibc

	blr
	.long 0
	.byte 0,0,0,1,128,0,0,0
	.size	._exit,.-._exit
	.size	.exit,.-.exit

diet_proto_weak(__thread_doexit)
diet_proto_weak(__fflush_stdin)
diet_proto_weak(__fflush_stdout)
diet_proto_weak(__fflush_stderr)
diet_proto_weak(flockfile)
diet_proto_weak(funlockfile)
diet_proto_weak(__nop)
diet_proto_weak(__you_tried_to_link_a_dietlibc_object_against_glibc)
diet_proto_weak(exit)
diet_proto(_exit)
