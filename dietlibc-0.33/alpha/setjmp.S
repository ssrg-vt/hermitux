#include <setjmp.h>

.text
.weak setjmp
.type setjmp,@function
setjmp:
.weak __setjmp
.type __setjmp,@function
__setjmp:
	mov	0, $17
.global __sigsetjmp
.type __sigsetjmp,@function
__sigsetjmp:
	stq	 $9, (JB_S0*8) ($16)	/* s0 */
	stq	$10, (JB_S1*8) ($16)	/* s1 */
	stq	$11, (JB_S2*8) ($16)	/* s2 */
	stq	$12, (JB_S3*8) ($16)	/* s3 */
	stq	$13, (JB_S4*8) ($16)	/* s4 */
	stq	$14, (JB_S5*8) ($16)	/* s5 */

	stq	$26, (JB_PC*8) ($16)	/* ra */
	stq	$fp, (JB_FP*8) ($16)	/* fp */
	stq	$sp, (JB_SP*8) ($16)	/* sp */

	stt	$f2, (JB_F2*8) ($16)	/* f2 */
	stt	$f3, (JB_F3*8) ($16)	/* f3 */
	stt	$f4, (JB_F4*8) ($16)	/* f4 */
	stt	$f5, (JB_F5*8) ($16)	/* f5 */
	stt	$f6, (JB_F6*8) ($16)	/* f6 */
	stt	$f7, (JB_F7*8) ($16)	/* f7 */
	stt	$f8, (JB_F8*8) ($16)	/* f8 */
	stt	$f9, (JB_F9*8) ($16)	/* f9 */

	br	$31, __sigjmp_save	/* jmp  __sigjmp_save */

