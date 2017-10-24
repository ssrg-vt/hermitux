.text
.align 0 
.global memcpy
.type	 memcpy,@function
memcpy:
	pushl %esi
	pushl %edi
	movl 12(%esp),%edi
	movl 16(%esp),%esi
	movl 20(%esp),%ecx
	movl %edi, %eax
	cld
	rep movsb
	popl %edi
	popl %esi
	ret
