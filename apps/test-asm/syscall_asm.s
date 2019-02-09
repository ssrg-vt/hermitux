	.global _start
	.global test
	.text

_start:
	mov $1, %rax
	mov $1, %rdi
	mov $message, %rsi
	mov $14, %rdx
	syscall

	mov $60, %rax
	xor	%rdi, %rdi
	syscall

message:
	.ascii "hello, world!\n"
