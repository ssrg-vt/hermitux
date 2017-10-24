#include "PIC.h"

.text
.global getenv
.type getenv,@function
getenv:
	pushl %esi
	pushl %edi
	movl 0xc(%esp), %edi
	cld
	movl %edi, %edx
	xorl %eax, %eax
	leal -1(%eax), %ecx
	repnz scasb
	not %ecx
	dec %ecx
	pushl %ecx
	movzbl (%edx), %ecx

	GET_1VAR environ, %esi		# non-PIC: movl environ, %esi
	jmp .Lloop

.Lprepare:
	movzbl (%edx), %ecx
	movl %eax, %esi
.Lloop:
	lodsl
	or %eax, %eax
	jz .Lout
	cmpb %cl, (%eax)
	jnz .Lloop

	movl %edx, %edi
	xchg %esi, %eax
	movl (%esp), %ecx
	repz cmpsb
	jne .Lprepare
	or %ecx, %ecx
	jnz .Lprepare
	cmpb $'=', (%esi)
	jne .Lprepare
	leal 0x1(%esi), %eax
.Lout:
	popl %ecx
	popl %edi
	popl %esi
	ret
.size	getenv, . - getenv
