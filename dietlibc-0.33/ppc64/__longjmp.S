#include <setjmp.h>

	.globl __longjmp
	.section	".opd","aw"
	.align 3
__longjmp:
		.quad	.__longjmp,.TOC.@tocbase,0
		.text
		.size	__longjmp,24
		.type	.__longjmp,@function
		.globl	.__longjmp
.align 2
.__longjmp:
	ld  1,(JB_GPR1*8)(3)
	ld  2,(JB_GPR2*8)(3)
	ld  0,(JB_LR*8)(3)
	ld  14,((JB_GPRS+0)*8)(3)
	lfd 14,((JB_FPRS+0)*8)(3)
	ld  15,((JB_GPRS+1)*8)(3)
	lfd 15,((JB_FPRS+1)*8)(3)
	ld  16,((JB_GPRS+2)*8)(3)
	lfd 16,((JB_FPRS+2)*8)(3)
	ld  17,((JB_GPRS+3)*8)(3)
	lfd 17,((JB_FPRS+3)*8)(3)
	ld  18,((JB_GPRS+4)*8)(3)
	lfd 18,((JB_FPRS+4)*8)(3)
	ld  19,((JB_GPRS+5)*8)(3)
	lfd 19,((JB_FPRS+5)*8)(3)
	ld  20,((JB_GPRS+6)*8)(3)
	lfd 20,((JB_FPRS+6)*8)(3)
	mtlr 0
	ld  21,((JB_GPRS+7)*8)(3)
	lfd 21,((JB_FPRS+7)*8)(3)
	ld  22,((JB_GPRS+8)*8)(3)
	lfd 22,((JB_FPRS+8)*8)(3)
	ld  0,(JB_CR*8)(3)
	ld  23,((JB_GPRS+9)*8)(3)
	lfd 23,((JB_FPRS+9)*8)(3)
	ld  24,((JB_GPRS+10)*8)(3)
	lfd 24,((JB_FPRS+10)*8)(3)
	ld  25,((JB_GPRS+11)*8)(3)
	lfd 25,((JB_FPRS+11)*8)(3)
	mtcrf 0xFF,0
	ld  26,((JB_GPRS+12)*8)(3)
	lfd 26,((JB_FPRS+12)*8)(3)
	ld  27,((JB_GPRS+13)*8)(3)
	lfd 27,((JB_FPRS+13)*8)(3)
	ld  28,((JB_GPRS+14)*8)(3)
	lfd 28,((JB_FPRS+14)*8)(3)
	ld  29,((JB_GPRS+15)*8)(3)
	lfd 29,((JB_FPRS+15)*8)(3)
	ld  30,((JB_GPRS+16)*8)(3)
	lfd 30,((JB_FPRS+16)*8)(3)
	ld  31,((JB_GPRS+17)*8)(3)
	lfd 31,((JB_FPRS+17)*8)(3)
	mr 3,4
	blr
.size .__longjmp,.-.__longjmp
