#include "PIC.h"

.text

.global expf,exp,expl,__finexp
	.type	 expf,@function
	.type	 exp,@function
	.type	 expl,@function
	.type	 __finexp,@function
expf:
	fldl2e
	fmuls 4(%esp)
	jmp .Lfinexp
expl:
	fldl2e
	fldt 4(%esp)
	fmulp
	jmp .Lfinexp
exp:
	fldl2e
	fmull 4(%esp)
#ifndef __DYN_LIB
__finexp:
#endif
.Lfinexp:
	fst	%st(1)
	frndint
	fst	%st(2)
	fsubrp
	f2xm1
	fld1
	faddp
	fscale
	ret

#ifdef __DYN_LIB
__finexp:
	PIC_RESTORE
	jmp .Lfinexp
#endif

.Lende:
.size    exp,.Lende-exp
.size    expl,.Lende-expl
.size    expf,.Lende-expf
.size    __finexp,.Lende-__finexp
