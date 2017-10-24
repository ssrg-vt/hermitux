#include "PIC.h"

.text

.global tanhf,tanh,tanhl
	.type	tanhf,@function
	.type	tanh,@function
	.type	tanhl,@function
tanhf:
	fldl2e
	fmuls 4(%esp)
	jmp fintanh
tanh:
	fldl2e
	fmull 4(%esp)
fintanh:
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call __finexp@PLT
	PIC_RESTORE
#else
	call __finexp
#endif
	fld1
	fdiv %st(1),%st	# st(0)=1/exp(x), st(1)=exp(x)
	fst %st(2)	# st(2)=1/exp(x)
	fadd %st(1),%st(0)
	fstp %st(3)	# st(2)=exp(x)+exp(-x), st(1)=exp(-x), st(0)=exp(x)
	fsubp		# st(1)=exp(x)+exp(-x), st(0)=exp(x)-exp(-x)
	fdivp
	ret

tanhl:
	fldl2e
	fldt 4(%esp)
	fmulp
	jmp fintanh

.Lende:
.size    tanh,.Lende-tanh
.size    tanhf,.Lende-tanhf
.size    tanhl,.Lende-tanhl
