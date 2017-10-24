#include <errno.h>
#include "syscalls.h"

	.text
	.align 4
	.global mmap
	.type	mmap,@function
mmap:
        /* Save registers and setup stack frame.  */
        stmg    %r6,%r15,48(%r15)
        lgr     %r1,%r15
        lg      %r0,8(%r15)             /* Load eos.  */
        aghi    %r15,-208               /* Buy stack space.  */
        stg     %r1,0(%r15)             /* Store back chain.  */
        stg     %r0,8(%r15)             /* Store eos.  */

        /* Store parameters on stack, because old_mmap 
           takes only one parameter: a pointer to the parameter area.  */
        mvc     200(8,%r15),368(%r15)   /* Move 'offset'.  */
        lgfr    %r6,%r6
        stg     %r6,192(%r15)           /* Store 'fd'.  */
        lgfr    %r5,%r5
        stg     %r5,184(%r15)           /* Store 'flags'.  */
        lgfr    %r4,%r4
        stg     %r4,176(%r15)           /* Store 'prot'.  */
        stg     %r3,168(%r15)           /* Store 'length'.  */
        stg     %r2,160(%r15)           /* Store 'start'.  */

        la      %r2,160(%r15)           /* Load address of parameter list.  */
        /* Do the system call trap.  */
        svc     __NR_mmap

        lg      %r15,0(%r15)            /* Load back chain.  */
        lmg     %r6,%r15,48(%r15)       /* Load registers.  */
	
	j __unified_syscall
