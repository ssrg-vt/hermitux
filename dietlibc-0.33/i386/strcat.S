.text
.align 0 
.global strcat
.type	 strcat,@function
strcat:
	pushl %edi
	pushl %esi

	movl 12(%esp), %edi
	movl 16(%esp), %esi

	pushl %edi

	cld
	xorl %eax, %eax
	xorl %ecx, %ecx
	decl %ecx
	repne scasb
	decl %edi
	
.Lloop:
	lodsb
	stosb
	testb %al, %al
	jnz .Lloop
	
	popl %eax
	popl %esi
	popl %edi
	ret
