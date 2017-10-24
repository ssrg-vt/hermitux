#include <setjmp.h>

#ifdef PIC
#define JUMPTARGET(name) name##@plt
#else
#define JUMPTARGET(name) .name
#endif


	.globl __sigsetjmp
	.section	".opd","aw"
	.align 3
__sigsetjmp:
		.quad	.__sigsetjmp,.TOC.@tocbase,0
		.text
		.size	__sigsetjmp,24
		.type	.__sigsetjmp,@function
		.globl	.__sigsetjmp
.align 2
.__sigsetjmp:
        std  1,(JB_GPR1*8)(3)
        mflr 0
        std  2,(JB_GPR2*8)(3)
        std  14,((JB_GPRS+0)*8)(3)
        stfd 14,((JB_FPRS+0)*8)(3)
        std  0,(JB_LR*8)(3)
        std  15,((JB_GPRS+1)*8)(3)
        stfd 15,((JB_FPRS+1)*8)(3)
        mfcr 0
        std  16,((JB_GPRS+2)*8)(3)
        stfd 16,((JB_FPRS+2)*8)(3)
        std  0,(JB_CR*8)(3)
        std  17,((JB_GPRS+3)*8)(3)
        stfd 17,((JB_FPRS+3)*8)(3)
        std  18,((JB_GPRS+4)*8)(3)
        stfd 18,((JB_FPRS+4)*8)(3)
        std  19,((JB_GPRS+5)*8)(3)
        stfd 19,((JB_FPRS+5)*8)(3)
        std  20,((JB_GPRS+6)*8)(3)
        stfd 20,((JB_FPRS+6)*8)(3)
        std  21,((JB_GPRS+7)*8)(3)
        stfd 21,((JB_FPRS+7)*8)(3)
        std  22,((JB_GPRS+8)*8)(3)
        stfd 22,((JB_FPRS+8)*8)(3)
        std  23,((JB_GPRS+9)*8)(3)
        stfd 23,((JB_FPRS+9)*8)(3)
        std  24,((JB_GPRS+10)*8)(3)
        stfd 24,((JB_FPRS+10)*8)(3)
        std  25,((JB_GPRS+11)*8)(3)
        stfd 25,((JB_FPRS+11)*8)(3)
        std  26,((JB_GPRS+12)*8)(3)
        stfd 26,((JB_FPRS+12)*8)(3)
        std  27,((JB_GPRS+13)*8)(3)
        stfd 27,((JB_FPRS+13)*8)(3)
        std  28,((JB_GPRS+14)*8)(3)
        stfd 28,((JB_FPRS+14)*8)(3)
        std  29,((JB_GPRS+15)*8)(3)
        stfd 29,((JB_FPRS+15)*8)(3)
        std  30,((JB_GPRS+16)*8)(3)
        stfd 30,((JB_FPRS+16)*8)(3)
        std  31,((JB_GPRS+17)*8)(3)
        stfd 31,((JB_FPRS+17)*8)(3)
        b JUMPTARGET (__sigjmp_save)
.size .__sigsetjmp,.-.__sigsetjmp
