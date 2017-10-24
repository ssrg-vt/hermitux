#include "PIC.h"

.text

.global ceilf,ceil,ceill,__flcetr
	.type ceilf,@function
	.type ceil,@function
	.type ceill,@function
	.type __flcetr,@function

ceilf:
	flds 4(%esp)
	movb $0x08,%ah
	jmp .Lflcetr

ceill:
	fldt 4(%esp)
	movb $0x08,%ah
	jmp .Lflcetr

ceil:
	fldl 4(%esp)
	movb $0x08,%ah

# Wspolny kod dla funkcji floor, ceil i trunc
# W ah maska bitow 11 i 10 rejestru sterowania koprocesora
#ifndef __DYN_LIB
__flcetr:
#endif
.Lflcetr:
	xorl %ecx,%ecx	# wyzerowanie rejestru
	movb %ah,%ch	# i utworzenie maski w cx
	pushl %eax	# krotsze niz subl $4,%esp
	fstcw (%esp)
	movw (%esp),%ax
	andb $0x03,%ah	# wyzerowanie bitow 11 i 10
	orl %ecx,%eax	# ustawienie bitow z maski
	movw %ax,2(%esp)
	fldcw 2(%esp)
	frndint
	fldcw (%esp)	# odtworzenie rejestru sterowania
	popl %eax	# i polozenia stosu
	ret

#ifdef __DYN_LIB
__flcetr:
	PIC_RESTORE
	jmp .Lflcetr
#endif

.Lende:
.size    ceil,.Lende-ceil
.size    ceill,.Lende-ceill
.size    ceilf,.Lende-ceilf
.size    __flcetr,.Lende-__flcetr
