.LEVEL	1.1

.text
.globl strncmp

strncmp:
	.PROC
	.CALLINFO
	.ENTRY
	ldb,ma 1(%r26), %r19
	ldb,ma 1(%r25), %r20
	addibt,= -1, %r24, .Lexit
	cmpb,=,n %r19, %r0,  .Lexit
	cmpb,=,n %r19, %r20, strncmp
	nop
.Lexit:
	bv %r0(%rp)
	sub %r19, %r20, %ret0
	.EXIT
	.PROCEND
	.EXPORT strncmp
