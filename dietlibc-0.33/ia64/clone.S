/* This is untested code which probably won´t work out of the box! */

#include "syscalls.h"
#include <errno.h>

.text
.globl __clone
.proc  __clone
.weak  __clone2
__clone2:
__clone:
	mov r8 = EINVAL
	cmp.eq p6,p0=0,r32     
	;;
(p6)	br.cond.spnt.few __error_unified_syscall
	cmp.eq p6,p0=0,r33     
	;;
(p6)	br.cond.spnt.few __error_unified_syscall
	flushrs
	;;

	mov.m r17 = ar.rsc
	mov r14 = r32
	mov r18 = r33
	mov r16 = r36;;
	mov r15 = __NR_clone2
	break 0x100000
	cmp.eq p6,p0=-1,r10
	;;
(p6)	br.cond.spnt.few __error_unified_syscall
	cmp.eq p6,p7=0,r8
	;;
(p6)	ld8 r34=[r14],8
(p6)	mov.m ar.bspstore=r18
(p6)	mov r32 = r16
	mov.m ar.rsc = r17
(p7)	br.ret.sptk b0
	;;
	ld8 r1 = [r14]
	mov b6 = r34
	br.call.dptk.few b0=b6
	mov r32 = r8
	br.call.dptk.few b0=__error_unified_syscall
	br.ret.sptk.few b0
.endp __clone
.size __clone, . - __clone
