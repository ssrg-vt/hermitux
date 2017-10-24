#include <errno.h>
#include "syscalls.h"

.text

@
@ mmap takes 6 parameters - ie more than can be passed in registers via the
@ regular syscall interface. Instead, parameters are passed on the stack.
@
@ On entry, the compiler will have already placed the fifth and sixth
@ parameters on the stack - all we need do here is push the first four and
@ call the syscall.
@

.global mmap

#ifdef __ARM_EABI__

mmap:
	str     r5, [sp, #-4]!
	ldr     r5, [sp, #8]
	str     r4, [sp, #-4]!
	ldr     r4, [sp, #8]
	mov	ip, r7
	mov	r7, #__NR_mmap2
	svc	0x00000000
	mov	r7, ip
	ldr     r4, [sp], #4
	ldr     r5, [sp], #4
	cmn	r0, #4096
	mov	pc, lr			@ return

#else	
		
mmap:
	stmdb	sp!, {r0, r1, r2, r3}
	mov	r0, sp
	swi	__NR_mmap
	add	sp, sp, #16
	b	__unified_syscall

#endif
