#include <syscalls.h>

.text
.global socketcall
.type	socketcall,@function
socketcall:
	leal 4(%esp), %ecx
	pushl %ecx
	movzbl %al,%eax
	pushl %eax
	movb $__NR_socketcall,%al
	call __unified_syscall
	popl %ecx
	popl %ecx
	retl
.Lende:
.size socketcall,.Lende-socketcall
