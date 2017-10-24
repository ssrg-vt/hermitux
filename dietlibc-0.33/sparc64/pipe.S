#include "syscalls.h"

.text
.global pipe
pipe:
	mov	%o0, %o2
	mov	__NR_pipe, %g1
	ta	0x6d
	bcs,pn	%xcc, 1f
	nop
	st	%o0, [ %o2 ]
	st	%o1, [ %o2 + 4 ]
	retl
	mov	%g0, %o0

1:	sethi	%hi(__unified_syscall_error_handler), %g1
	jmp	%g1+%lo(__unified_syscall_error_handler)
	nop
