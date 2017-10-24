#include "PIC.h"

.text

.global __write1
.type	__write1,@function

.global __write2
.type	__write2,@function

					# regparm=1, daher Stringadresse in %eax
__write2:
	clc				# CY = 0
	.byte	0xB2			# zusammen mit nächstem Byte: mov dl,0xF9
__write1:
	stc				# CY = 1
	sbbl	%ecx,%ecx		# __write2: ecx=0, __write1: ecx=-1
	incl	%ecx
	incl	%ecx			# __write2: ecx=2, __write1: ecx=1
	xorl	%edx,%edx
	decl	%edx
.Lnext:	incl	%edx
	cmpb	%ch,(%edx,%eax)		# ch=0, da bei beiden Filedescriptoren Bits 15:8 0 sind
	jnz	.Lnext			# Stringlänge in edx, ohne eax zerstört zu haben
	PIC_SAVE		# non-PIC: empty line
	PIC_INIT		# non-PIC: empty line
	pushl	%edx
	pushl	%eax
	pushl	%ecx
#ifdef __DYN_LIB
	call	write@PLT
#else
	call	write			# alles ruf uf dn Stack und ab damit
#endif
	addl	$12,%esp		# und das leidvolle Putzen
	PIC_RESTORE		# non-PIC: empty line
	ret
