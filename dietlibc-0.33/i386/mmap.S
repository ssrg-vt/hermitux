#include "syscalls.h"

.text
.global mmap
.type	mmap,@function
mmap:
	mov	$__NR_mmap,%al
	lea	0x4(%esp,1),%edx
	push	%edx
	call	__unified_syscall
	pop	%ecx
	ret
.Lende3:
.size mmap,.Lende3-mmap
