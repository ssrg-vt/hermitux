.LEVEL	1.1

.text
.globl strncpy

strncpy:
	.PROC
	.CALLINFO
	.ENTRY
	ldb,ma 1(%r25), %r19
	addi,= -1, %r24, %r24
	cmpb,<> %r0, %r19, strncpy
	stb,ma %r19, 1(%r26)
	bv,n %r0(%rp)
	.EXIT
	.PROCEND
	.EXPORT strncpy
