#include "PIC.h"

.text

.global coshf,cosh,coshl
	.type	coshf,@function
	.type	cosh,@function
	.type	coshl,@function
coshf:
	fldl2e
	fmuls 4(%esp)
	jmp fincosh
cosh:
	fldl2e
	fmull 4(%esp)
fincosh:
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call __finexp@PLT
	PIC_RESTORE
#else
	call __finexp
#endif
	fld1
	fdiv %st(1),%st(0)	# st(0)=1/exp(x), st(1)=exp(x)
	faddp %st,%st(1)
	pushl $0x3f000000	# 1/2
	flds (%esp)
	popl %eax
	fmulp
	ret

coshl:
	fldl2e
	fldt 4(%esp)
	fmulp
	jmp fincosh

.Lende:
.size    cosh,.Lende-cosh
.size    coshf,.Lende-coshf
.size    coshl,.Lende-coshl
