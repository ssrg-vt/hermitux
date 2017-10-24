.text
.weak setjmp
setjmp:
.global __setjmp
__setjmp:
	mov	r1, #0
.global __sigsetjmp
__sigsetjmp:
.weak sigsetjmp
sigsetjmp:
#ifndef	__SOFTFP__
	sfm	f4, 4, [r0], #48
#endif
	stmia	r0, {r4-r11, sp, lr}
	sub	r0, r0, #48
	b	__sigjmp_save

