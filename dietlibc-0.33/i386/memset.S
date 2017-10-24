.text
.align 0
.global memset
.type	 memset,@function
memset:
	pushl %edi
	movl 8(%esp),%edi
	movl 12(%esp),%eax
	movl 16(%esp),%ecx
	cld
	pushl %edi
	rep stosb
	popl %eax
	popl %edi
	ret
