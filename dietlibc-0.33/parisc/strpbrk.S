.level 1.1

.text
.globl strpbrk

strpbrk:
	.PROC
	.CALLINFO
	.ENTRY
	ldb,ma 1(%r26), %r19
	cmpb,=,n %r0, %r19, .Leret
	copy %r25, %r23
	ldb,ma 1(%r23), %r20
.Lcmp:
	cmpb,=,n %r0, %r20, strpbrk
	nop
	cmpb,<>,n %r19, %r20, .Lcmp
	ldb,ma 1(%r23), %r20
	bv %r0(%rp)
	addi -1, %r26, %ret0
.Leret:
	bv %r0(%rp)
	ldi 0, %ret0
	.EXIT
	.PROCEND
	.EXPORT strpbrk
