#include <syscalls.h>

.text
.align 2
.weak waitpid
waitpid:
.global __libc_waitpid
__libc_waitpid:
	clr	$19
	br	wait4
