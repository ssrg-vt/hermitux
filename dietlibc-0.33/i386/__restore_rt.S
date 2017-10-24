#include "syscalls.h"

.text
.type   __restore_rt,@function
.global __restore_rt
.align 8
__restore_rt:
	movl $__NR_rt_sigreturn,%eax
	int $0x80
	hlt	/* die if syscall returns */
.size    __restore_rt,.-__restore_rt

.text
.type   __restore,@function
.global __restore
.align 8
__restore:
	popl %eax
	movl $__NR_sigreturn,%eax
	int $0x80
	hlt	/* die if syscall returns */
.size    __restore,.-__restore
