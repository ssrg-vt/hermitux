#include "PIC.h"

.text
.global usleep
.type    usleep,@function

usleep:
	movl	4(%esp),%eax
	PIC_SAVE		# non-PIC: empty line
	PIC_INIT		# non-PIC: empty line
	xorl	%edx,%edx
	movl	$1000000,%ecx
	divl	%ecx
	imull	$1000,%edx
	pushl	%edx
	pushl	%eax
	movl	%esp,%eax
	pushl	%eax
	pushl	%eax
#ifdef __DYN_LIB
	call	nanosleep@PLT
#else
	call	nanosleep
#endif
	addl	$16,%esp
	PIC_RESTORE		# non-PIC: empty line
	ret

.Lende:
.size    usleep,.Lende-usleep

