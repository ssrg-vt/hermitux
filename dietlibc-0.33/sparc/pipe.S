#include <dietfeatures.h>
#include "syscalls.h"

.text
.global pipe
pipe:
	mov	%o0, %o2
	mov	__NR_pipe, %g1
	ta	0x10
	bcs	__unified_syscall_error_handler
	nop
	st	%o0, [ %o2 ]
	st	%o1, [ %o2 + 4 ]
	retl
	mov	%g0, %o0

