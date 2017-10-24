#include <setjmp.h>

.text
.align 2

.global __longjmp
.type __longjmp,@function
__longjmp:
	mov	$17, $0 	/* a1 -> v0 */

	ldq	 $9, (JB_S0*8) ($16)	/* s0 */
	ldq	$10, (JB_S1*8) ($16)	/* s1 */
	ldq	$11, (JB_S2*8) ($16)	/* s2 */
	ldq	$12, (JB_S3*8) ($16)	/* s3 */
	ldq	$13, (JB_S4*8) ($16)	/* s4 */
	ldq	$14, (JB_S5*8) ($16)	/* s5 */

	ldq	$26, (JB_PC*8) ($16)	/* ra */
	ldq	$fp, (JB_FP*8) ($16)	/* fp */
	ldq	$sp, (JB_SP*8) ($16)	/* sp */

	ldt	$f2, (JB_F2*8) ($16)	/* f2 */
	ldt	$f3, (JB_F3*8) ($16)	/* f3 */
	ldt	$f4, (JB_F4*8) ($16)	/* f4 */
	ldt	$f5, (JB_F5*8) ($16)	/* f5 */
	ldt	$f6, (JB_F6*8) ($16)	/* f6 */
	ldt	$f7, (JB_F7*8) ($16)	/* f7 */
	ldt	$f8, (JB_F8*8) ($16)	/* f8 */
	ldt	$f9, (JB_F9*8) ($16)	/* f9 */

	cmoveq	 $0, 0x1, $0

	ret	$31, ($26), 1
