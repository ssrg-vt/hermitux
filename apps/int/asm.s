	.global asm_loop
	.text

asm_loop:
	mov $0, %rcx

loop1:

	;int $3
	mov $42, %rax
	syscall

	inc %rcx
	cmp $10000000, %rcx
	jne loop1
	ret
