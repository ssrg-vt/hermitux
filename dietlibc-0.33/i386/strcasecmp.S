.text
.global strcasecmp
.type	strcasecmp,@function

strcasecmp:
	pushl %esi
	movl 0x8(%esp), %esi
	movl 0xc(%esp), %edx
	xorl %eax, %eax
	xorl %ecx, %ecx
	cld
.Lloop:
	lodsb
	movb (%edx), %cl
	incl %edx
	or %al, %al
	jz .Lfinifirst
	cmp $'A', %al
	jnge .Lcmp
	cmp $'z', %al
	jg .Lcmp
	or $0x20, %al
	or $0x20, %cl
.Lcmp:
	subl %ecx, %eax
	jz .Lloop
.Lret:
	popl %esi
	ret
.Lfinifirst:
	subl %ecx, %eax
	jmp .Lret

.size strcasecmp,.-strcasecmp
