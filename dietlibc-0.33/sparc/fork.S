#include <dietfeatures.h>
#include "syscalls.h"

.text
.weak fork
fork:
.global __libc_fork
__libc_fork:
	mov	__NR_fork, %g1
	ta	0x10
	bcs	__unified_syscall_error_handler
	dec	%o1
	retl
	and	%o0, %o1, %o0

