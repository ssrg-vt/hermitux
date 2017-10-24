#include "PIC.h"

.text

.global sinhf,sinh,sinhl
	.type	sinhf,@function
	.type	sinh,@function
	.type	sinhl,@function
sinhf:
	fldl2e
	fmuls 4(%esp)
	jmp finsinh
sinh:
	fldl2e
	fmull 4(%esp)
finsinh:
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
	fsubrp %st(0),%st(1)
	pushl $0x3f000000	# 1/2
	flds (%esp)
	popl %eax
	fmulp
	ret

sinhl:
	fldl2e
	fldt 4(%esp)
	fmulp
	jmp finsinh

.Lende:
.size    sinh,.Lende-sinh
.size    sinhf,.Lende-sinhf
.size    sinhl,.Lende-sinhl
