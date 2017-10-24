#include <errno.h>
#include "syscalls.h"

#define EINVAL  22

	.text
	.align 4
	.global mmap
	.type	mmap,@function
mmap:
        /* Save registers and setup stack frame.  */
        stm     %r6,%r15,24(%r15)
        lr      %r1,%r15
        ahi     %r15,-120               /* buy stack space */
        st      %r1,0(%r15)             /* store back chain */

        /* Store parameters on stack, because old_mmap/mmap2
         * take only one parameter: a pointer to the parameter area
         */
        st      %r6,0x70(%r15)          /* Store 'fd'.      */
        st      %r5,0x6C(%r15)          /* Store 'flags'.   */
        st      %r4,0x68(%r15)          /* Store 'prot'.    */
        st      %r3,0x64(%r15)          /* Store 'length'.  */
        st      %r2,0x60(%r15)          /* Store 'start'.   */
        l       %r1,216(%r15)           /* Load offset.     */

        tml     %r1,0x0fff              /* Offset page aligned ?  */
        lhi     %r2,-EINVAL
        jnz     1f                      /* No -> EINVAL.  */
        srl     %r1,12                  /* mmap2 takes the offset in pages.  */
        st      %r1,0x74(%r15)          /* Store page offset.  */
        la      %r2,0x60(%r15)          /* Load address of parameter list.  */
        svc     __NR_mmap2              /* Do the system call trap.  */

1:      l       %r15,0(%r15)            /* Load back chain.  */
        lm      %r6,%r15,24(%r15)       /* Load registers.  */

	j	__unified_syscall

        /* check gpr 2 for error */
        lhi     %r0,-4096
        clr     %r2,%r0
        jnl     1b

        /* Successful; return the syscall's value.  */
        br      %r14

