
.text
.align 0 
.global strcpy
.type	 strcpy,@function
strcpy:
	pushl %esi
	pushl %edi
	
	movl 12(%esp), %edx
	movl 16(%esp), %esi
	movl %edx, %edi
	cld

.Lloop:
	lodsb
	stosb
	orb %al, %al
	jnz .Lloop

	popl %edi
	popl %esi
	movl %edx,%eax
	ret
