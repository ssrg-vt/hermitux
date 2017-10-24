.text
.align 2
.global __testandset
.type	__testandset,@function
__testandset:
1:	ldq_l	$0, 0($16)		/* load lock */
	bne	$0, 1f			/* ok there is a lock... */
	lda	$0, 1($31)		/* load a 1 to register 0 */
	stq_c	$0, 0($16)		/* write lock is locked :) */
	beq	$0, 1b			/* oops someone changed MY lock */
1:	ret	$31, ($26), 0x01	/* return */
