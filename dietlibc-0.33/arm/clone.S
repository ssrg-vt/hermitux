
#include <errno.h>
#include "syscalls.h"

	.text
	.weak 	clone
	.global __clone
	
@
@ Some slightly tricky stuff here... edit with care :-)
@


#ifdef __ARM_EABI__

#define CLONE_VM      0x00000100
#define CLONE_THREAD  0x00010000
	
@ ;  don't do this yet
@#define RESET_PID

		
clone:
__clone:
	@ ; start with a sanity check
	cmp	r0, #0
	cmpne	r1, #0
	mvneq	r0, #-EINVAL
	beq	__unified_syscall	@ handle as if error was returned by the syscall

	@ ; insert args into stack
	str	r3, [r1, #-4]!
	str	r0, [r1, #-4]!
	
	@ ; do the system call
	@ ; get the flags
	mov	r0, r2
#ifdef RESET_PID	
	mov	ip, r2
#endif	
	@ ; child sp is already in r1
	stmfd	sp!, {r4, r7}
	@stmdb	sp!, {r4, r7}
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	ldr	r4, [sp, #16]
        ldr     r7, =__NR_clone
	swi	0
	
	cmp	r0, #0
	beq	1f
	ldmfd	sp!, {r4, r7}	
	blt	__unified_syscall	@ (return code < 0): handle as an error
	bx	lr
1:	
#ifdef RESET_PID		
	tst	ip, #CLONE_THREAD
	bne	3f

	mov	r0, #0xffff0fff
	mov	lr, pc
	sub	pc, r0, #31
	mov	r1, r0
	tst	ip, #CLONE_VM
	movne	r0, #-1
        ldr     r7, =__NR_getpid
	swi	0
	str	r0, [r1, #PID_OFFSET] @	; not defined yet ??
	str	r0, [r1, #TID_OFFSET] @	; not defined yet ??
3:	
#endif
	@ ; pick the function arg and call address off the stack and execute
	ldr	r0, [sp, #4]
	mov	lr, pc
	ldr	pc, [sp], #8
	
	@ ; and we're done, passing return value through r0
	b	_exit			@ branch to _exit (PIC safe)

	
#else
		
clone:
__clone:
	movs	r12, r0			@ check function pointer
	cmpne	r1, #0			@ if function check for stack pointer
	moveq	r0, #-EINVAL		@ if one is not available set errno value
	beq	__unified_syscall	@ handle as if error was returned by the syscall

	stmdb	r1!, { r3, r12 } 	@ store function param (r3) and function ptr (r12)
					@ into what will become the childs stack.

	mov	r0, r2			@ move flags argument to r0
	swi	__NR_clone		@ call the syscall
	movs	r0, r0			@ set condition codes
	blt	__unified_syscall	@ (return code < 0): handle as an error
	movne	pc, lr			@ (return code > 0): return to parent

	mov	fp, #0			@ clear the frame pointer
	adr	lr, 1f			@ setup return address
	ldmia	sp!, { r0, pc } 	@ load function param and jump to thread function

1:	b	_exit			@ branch to _exit (PIC safe)

#endif
