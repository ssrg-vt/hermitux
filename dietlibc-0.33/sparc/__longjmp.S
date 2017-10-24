#include <setjmp.h>

#define JB_SP  0
#define JB_FP  1
#define JB_PC  2

#define ENV(base,reg) [%base + (reg * 4)]
#define ST_FLUSH_WINDOWS 3
#define RW_FP [%fp + 0x48]

.text
.global __longjmp
.type __longjmp,function
__longjmp:
	/* Store our arguments in global registers so we can still
	   use them while unwinding frames and their register windows.  */

	ld ENV(o0,JB_FP), %g3   /* Cache target FP in register %g3.  */
	mov %o0, %g1            /* ENV in %g1 */
	orcc %o1, %g0, %g2      /* VAL in %g2 */
	be,a 0f                 /* Branch if zero; else skip delay slot.  */
	 mov 1, %g2             /* Delay slot only hit if zero: VAL = 1.  */
0:
	xor %fp, %g3, %o0
	add %fp, 512, %o1
	andncc %o0, 4095, %o0
	bne .Lthread
	 cmp %o1, %g3
	bl .Lthread

	/* Now we will loop, unwinding the register windows up the stack
	   until the restored %fp value matches the target value in %g3.  */

.Lloop:
	cmp %fp, %g3            /* Have we reached the target frame? */
	bl,a .Lloop             /* Loop while current fp is below target.  */
	 restore                /* Unwind register window in delay slot.  */
	be,a .Lfound            /* Better have hit it exactly.  */
	 ld ENV(g1,JB_SP), %o0  /* Delay slot: extract target SP.  */

.Lthread:
	/*
	 * Do a "flush register windows trap".  The trap handler in the
	 * kernel writes all the register windows to their stack slots, and
	 * marks them all as invalid (needing to be sucked up from the
	 * stack when used).  This ensures that all information needed to
	 * unwind to these callers is in memory, not in the register
	 * windows.
	 */
	ta      ST_FLUSH_WINDOWS
	ld      ENV(g1,JB_PC), %o7 /* Set return PC. */
	ld      ENV(g1,JB_SP), %fp /* Set saved SP on restore below. */
	sub     %fp, 64, %sp    /* Allocate a register frame. */
	st      %g3, RW_FP      /* Set saved FP on restore below. */
	retl
	 restore %g2, 0, %o0    /* Restore values from above register frame. */

.Lfound:
	/* We have unwound register windows so %fp matches the target.  */
	mov %o0, %sp            /* OK, install new SP.  */

.Lsp_ok:
	ld ENV(g1,JB_PC), %o0   /* Extract target return PC.  */
	jmp %o0 + 8             /* Return there.  */
	 mov %g2, %o0           /* Delay slot: set return value.  */

.size __longjmp, . - __longjmp

