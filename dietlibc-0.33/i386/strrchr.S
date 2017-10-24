.text
.globl strrchr
.type strrchr,@function

strrchr:
	movl	0x4(%esp), %edx
	movb	0x8(%esp), %cl
	xorl	%eax, %eax
.Lloop:
	cmpb	%cl, (%edx)
	jnz	.Ltest0
	movl	%edx, %eax
.Ltest0:
	cmpb	$0,(%edx)
	jz	.Lret
	incl	%edx
	jmp	.Lloop
.Lret:
	ret
