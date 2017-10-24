.text
.global gettimeofday
.type gettimeofday,@function
gettimeofday:
	movq gtod(%rip), %rax
	test %rax,%rax
	jz .L1
.L2:
	callq *%rax
	jmp __error_unified_syscall
.L1:
	push %rsi
	push %rdi
	movq __vdso(%rip),%rdi
	movq $funcname, %rsi
	call vdso_dlsym
	pop %rdi
	pop %rsi
	jmp .L2

.Lhere:
	.size	 gettimeofday,.Lhere-gettimeofday
.local gtod
.comm gtod,8,8
.section .rodata.str1.1,"aMS",@progbits,1
.local funcname
funcname:
	.string "__vdso_gettimeofday"
