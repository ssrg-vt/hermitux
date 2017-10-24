#include "PIC.h"

.text

.global floorf,floor,floorl
	.type floorf,@function
	.type floor,@function
	.type floorl,@function

floorf:
	flds 4(%esp)
	movb $0x04,%ah
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	jmp __flcetr@PLT
#else
	jmp __flcetr
#endif

floor:
	fldl 4(%esp)
	movb $0x04,%ah
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	jmp __flcetr@PLT
#else
	jmp __flcetr
#endif

floorl:
	fldt 4(%esp)
	movb $0x04,%ah
#ifdef __DYN_LIB
	PIC_SAVE
	PIC_INIT
	jmp __flcetr@PLT
#else
	jmp __flcetr
#endif

.Lende:
.size    floor,.Lende-floor
.size    floorl,.Lende-floorl
.size    floorf,.Lende-floorf
