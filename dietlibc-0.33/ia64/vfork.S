#include "syscalls.h"

.text
.globl vfork
vfork:
	mov r15 = __NR_clone
	mov r32 = 16657
	br __unified_syscall
