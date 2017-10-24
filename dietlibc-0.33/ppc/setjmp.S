#include <setjmp.h>

#ifdef PIC
#define JUMPTARGET(name) name##@plt
#else
#define JUMPTARGET(name) name
#endif

.global __sigsetjmp
.type __sigsetjmp,@function
.align 2
__sigsetjmp:
        stw  1,(JB_GPR1*4)(3)
        mflr 0
        stw  2,(JB_GPR2*4)(3)
        stw  14,((JB_GPRS+0)*4)(3)
        stfd 14,((JB_FPRS+0*2)*4)(3)
        stw  0,(JB_LR*4)(3)
        stw  15,((JB_GPRS+1)*4)(3)
        stfd 15,((JB_FPRS+1*2)*4)(3)
        mfcr 0
        stw  16,((JB_GPRS+2)*4)(3)
        stfd 16,((JB_FPRS+2*2)*4)(3)
        stw  0,(JB_CR*4)(3)
        stw  17,((JB_GPRS+3)*4)(3)
        stfd 17,((JB_FPRS+3*2)*4)(3)
        stw  18,((JB_GPRS+4)*4)(3)
        stfd 18,((JB_FPRS+4*2)*4)(3)
        stw  19,((JB_GPRS+5)*4)(3)
        stfd 19,((JB_FPRS+5*2)*4)(3)
        stw  20,((JB_GPRS+6)*4)(3)
        stfd 20,((JB_FPRS+6*2)*4)(3)
        stw  21,((JB_GPRS+7)*4)(3)
        stfd 21,((JB_FPRS+7*2)*4)(3)
        stw  22,((JB_GPRS+8)*4)(3)
        stfd 22,((JB_FPRS+8*2)*4)(3)
        stw  23,((JB_GPRS+9)*4)(3)
        stfd 23,((JB_FPRS+9*2)*4)(3)
        stw  24,((JB_GPRS+10)*4)(3)
        stfd 24,((JB_FPRS+10*2)*4)(3)
        stw  25,((JB_GPRS+11)*4)(3)
        stfd 25,((JB_FPRS+11*2)*4)(3)
        stw  26,((JB_GPRS+12)*4)(3)
        stfd 26,((JB_FPRS+12*2)*4)(3)
        stw  27,((JB_GPRS+13)*4)(3)
        stfd 27,((JB_FPRS+13*2)*4)(3)
        stw  28,((JB_GPRS+14)*4)(3)
        stfd 28,((JB_FPRS+14*2)*4)(3)
        stw  29,((JB_GPRS+15)*4)(3)
        stfd 29,((JB_FPRS+15*2)*4)(3)
        stw  30,((JB_GPRS+16)*4)(3)
        stfd 30,((JB_FPRS+16*2)*4)(3)
        stw  31,((JB_GPRS+17)*4)(3)
        stfd 31,((JB_FPRS+17*2)*4)(3)
        b JUMPTARGET (__sigjmp_save)
.size __sigsetjmp,.-__sigsetjmp
