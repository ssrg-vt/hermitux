#include "dietfeatures.h"

.text
	.align	2
	.global	strcpy

strcpy:
#ifndef WANT_SMALL_STRING_ROUTINES
	mov	ip, r0
	ands	r2, r1, #3
	ldr	r3, [r1, -r2]!
	movne	r2, r2, lsl #3
#ifdef __ARMEB__
	movne	r3, r3, lsl r2
#else
	movne	r3, r3, lsr r2
#endif
	addne	pc, pc, r2, lsl #1
	mov	r0, r0			@ nop
.Lloop:
#ifdef __ARMEB__
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	ldrne	r3, [r1, #4]!
#else
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, lsr #8
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, lsr #8
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, lsr #8
	tst	r3, #255
	strb	r3, [ip], #1
	ldrne	r3, [r1, #4]!
#endif
#else
	ldrb	r2, [r1], #1
	mov	r3, r0
.Lloop:
	strb	r2, [r3], #1
	tst	r2, #255
	ldrneb	r2, [r1], #1
#endif
	bne	.Lloop
	mov	pc, lr
.Lfe1:
	.size	 strcpy,.Lfe1-strcpy
