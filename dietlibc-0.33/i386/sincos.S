#include "PIC.h"

.text

.global sincosf,sincos,sincosl
.type   sincosf,@function
.type   sincos,@function
.type   sincosl,@function
# void  sincos ( double x, double* sinx, double* cosx );

sincosf:
        flds     4(%esp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__fmod2pi@PLT
	PIC_RESTORE
#else
	call	__fmod2pi
#endif
        movl    12(%esp),%eax
        movl    8(%esp),%ecx
        fsincos
        fstps   (%eax)
        fstps   (%ecx)
        ret
sincos:
        fldl     4(%esp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__fmod2pi@PLT
	PIC_RESTORE
#else
	call	__fmod2pi
#endif
        movl    16(%esp),%eax
        movl    12(%esp),%ecx
        fsincos
        fstpl   (%eax)
        fstpl   (%ecx)
        ret
sincosl:
        fldt     4(%esp)
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	call	__fmod2pi@PLT
	PIC_RESTORE
#else
	call	__fmod2pi
#endif
        movl    20(%esp),%eax
        movl    16(%esp),%ecx
        fsincos
        fstpt   (%eax)
        fstpt   (%ecx)
        ret


.ende:
.size    sincos,.ende-sincos
.size    sincosf,.ende-sincosf
.size    sincosl,.ende-sincosl
