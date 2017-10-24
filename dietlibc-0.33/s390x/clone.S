#include <errno.h>
#include "syscalls.h"

.text
.align 4
.global __clone
.type	__clone,@function
__clone:
	ltr	%r1, %r2
	lhi	%r2, -EINVAL
	jz	.Lerror
	ltr	%r3, %r3
	jz	.Lerror
	lr	%r2, %r3
	lr	%r3, %r4
	svc	__NR_clone
	ltr	%r2, %r2
	jm	.Lerror
	jz	thread_start
	br	%r14

thread_start:
	lr	%r2, %r5
	sr	%r11,%r11
	ahi	%r15,-96
	basr	%r14,%r1

	basr	%r1, %r0
.L0:	al	%r1, [.L1-.L0](%r1)
	br	%r1
.L1:	.long	_exit-.L0

.Lerror:
	basr	%r1, %r0
.L2:	al	%r1, [.L3-.L2](%r1)
	br	%r1
.L3:	.long	__error_unified_syscall-.L0
