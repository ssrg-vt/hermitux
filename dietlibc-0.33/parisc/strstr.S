.level 1.1

.text
.globl strstr
strstr:
	.PROC
	.CALLINFO
	.ENTRY
	ldb,ma 1(%r25), %r20
	ldb,ma 1(%r26), %r19
	ldi 2, %ret0
.L1:
	cmpb,=,n %r0, %r19, .Lret2
	cmpb,<> %r20, %r19, .L1
	ldb,ma 1(%r26), %r19
	copy %r19, %r24
	copy %r25, %r23
	ldb,ma 1(%r23), %r21
	copy %r26, %r22
.Lloop:
	cmpb,<> %r24, %r21, .L1
	ldb,ma 1(%r22), %r24
	cmpb,= %r0, %r24, .Lret
	ldb,ma 1(%r23), %r21
	cmpb,<>,n %r21, %r0, .Lloop
	nop
.Lret:
	addi 0, %r26, %ret0
.Lret2:
	bv %r0(%rp)
	addi -2, %ret0, %ret0
	.EXIT
	.PROCEND
	.EXPORT strstr
