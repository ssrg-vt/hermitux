#include "syscalls.h"

.text
.globl pipe
.proc  pipe

pipe:
	st8  [r12] = r32
	mov   r15  = __NR_pipe
	break.i  0x100000
	ld8 r2 = [r12]
	cmp.eq p7,p6=-1,r10
	;;
(p6)	st4 [r2] = r8,4
	mov r8 = r0
	;;
(p7)	br.cond.spnt.few __error_unified_syscall
(p6)	st4 [r2] = r9
(p6)	br.ret.sptk.few b0

.endp pipe
.size pipe, . - pipe
