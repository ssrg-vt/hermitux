.text
.align 2
.global getppid
getppid:
	lda	$sp,-8($sp)
	stq	$26, 0($sp)
	jsr	getpid
	ldq	$26, 0($sp)
	lda	$sp, 8($sp)
	cmovge	$0, $20, $0
	ret
