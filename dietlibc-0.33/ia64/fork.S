#include "syscalls.h"
#include <errno.h>
#define SIGCLD 17

	.text
	.globl fork
	.proc  fork
	.weak  __libc_fork
	.type fork,@function
	.type __libc_fork,@function
fork:
__libc_fork:
	alloc r2=ar.pfs,0,0,2,0
	mov out0=SIGCLD
	mov out1=0
	;;
	mov r15 = __NR_clone
	br __unified_syscall
