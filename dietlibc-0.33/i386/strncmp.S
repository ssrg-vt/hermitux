.text
.global strncmp
.type	strncmp,function
strncmp:
	pushl	%esi
	pushl	%edi
	movl	%esp,%ecx
	movl	12(%ecx),%esi
	movl	16(%ecx),%edi
	movl	20(%ecx),%ecx
	jecxz	.Lequal
.Lloop:
	movzbl	(%esi),%eax
	movzbl	(%edi),%edx
	incl	%esi
	incl	%edi
	/* !equal ? */
	subl	%edx,%eax
	jnz	.Lout
	/* end of c-string ? */
	test	%edx,%edx
	jz	.Lequal
	/* do loop */
	decl	%ecx
	jnz	.Lloop

.Lequal:
	xorl	%eax,%eax
.Lout:
	popl %edi
	popl %esi
	ret
.size	strncmp,.-strncmp
