#include "syscalls.h"

.text
.weak fork
fork:
.global __libc_fork
__libc_fork:
	mov	2, %g1
	ta	0x6d
	bcs	%xcc, 1f
	nop
	dec	%o1
	retl
	and	%o0, %o1, %o0

1:	sethi	%hi(__unified_syscall_error_handler), %g1
	jmp	%g1+%lo(__unified_syscall_error_handler)
	nop
