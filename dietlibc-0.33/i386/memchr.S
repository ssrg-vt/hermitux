.text
.global memchr
.type memchr,@function
memchr:
	push %edi
	mov 8(%esp),%edi	/* edi = ptr */
	movzxb 12(%esp),%eax
	mov 16(%esp),%ecx
	imul $0x01010101,%eax	/* eax = simd vector, 8 x ch */
	push %esi
	push %ebx
	jecxz .Lnull
	add %edi,%ecx		/* ecx == max ptr */

1:	/* compare byte-wise until alignment */
	test $3,%edi
	jz 1f
	cmp %al,(%edi)
	jz .Lfound
	inc %edi
	jmp 1b

1:	/* compare word-wise */
	cmp %edi,%ecx
	jb .Lnull
	mov (%edi),%edx
	xor %eax,%edx		/* in the input vector, change the byte we want to 0 */
	mov %edx,%ebx
	sub $0x01010101,%ebx
	not %edx
	and %ebx,%edx
	shr $7,%edx
	and $0x01010101,%edx	/* this sequence did !! to every byte in edx */
	jnz 1f
	add $4,%edi
	jmp 1b

1:	/* there is a nonzero byte in %edx, try lower half */
	test %dx,%dx
	jnz 1f
	/* not in lower half, swap halves */
	shr $16,%edx
	add $2,%edi
1:
	test %dl,%dl
	jnz 1f
	/* no match in lower byte */
	mov %dh,%dl
	inc %edi
1:

.Lfound:
	cmp %edi,%ecx
	jbe .Lnull
	mov %edi,%eax
	pop %ebx
	pop %esi
	pop %edi
	ret

.Lnull:
	xor %eax,%eax
	pop %ebx
	pop %esi
	pop %edi
	ret
.Lhere:
.size memchr,.Lhere-memchr
