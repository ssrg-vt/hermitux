.text
.global __testandset
.type	__testandset,@function
__testandset:
	xorl	%eax, %eax
	movl	0x4(%esp),%edx
	incl	%eax
	xchgl	%eax,(%edx)
	ret
