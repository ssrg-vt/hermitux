/* taken from glibc 2.2 */

	.set noreorder
	.set noat

	.ent __remqu
	.globl __remqu

	.align 3
__remqu :
	lda	$30 , - 32 ($30 )
	.frame	$30 , 32 , $23  , 0
	.prologue 0
.Ludiv:
	stq	$1  , 0($30 )
	mov	$25 ,  $1
	stq	$2  , 8($30 )
	mov	$24 ,  $27
	stq	$0  , 16($30 )
	clr	$2
	stq	$3  , 24($30 )
	ldiq	$0  , 1

	beq	$1  , .Ldivbyzero

	.align 3

1:	cmpult	$1  , $27   , $at
	blt	$1  , 2f
	addq	$1  , $1  , $1
	addq	$0  , $0  , $0
	bne	$at , 1b
	unop
2:
3:
	srl	$0  , 1, $0
	cmpule	$1  , $27   , $at
	subq	$27 , $1  , $3

	srl	$1  , 1, $1
	cmovne	$at , $3  , $27
	bne	$0  , 3b

.Ldone:	ldq	$1  , 0($30 )
	ldq	$2  , 8($30 )
	ldq	$0  , 16($30 )
	ldq	$3  , 24($30 )

	lda	$30 , 32 ($30 )
	ret	$31 , ($23  ), 1

.Ldivbyzero:
	mov	$16 , $3
	ldiq	$16 , -2
	call_pal 170
	mov	$3  , $16
	clr	$27
	br	.Ldone

	.end __remqu

	.ent __remq
	.globl __remq

	.align 3
__remq :
	lda	$30 , - 32 ($30 )
	.frame	$30 , 32 , $23  , 0
	.prologue 0
	or	$24  , $25  , $at

	bge	$at , .Ludiv

	stq	$24 , 0($30 )
	negq	$24 , $at
	stq	$25 , 8($30 )
	cmovge	$at , $at , $24
	stq	$23 , 16($30 )
	negq	$25 , $at
	stq	$3  , 24($30 )
	cmovge	$at , $at , $25

	bsr	$23 , __remqu

	ldq	$24 , 0($30 )
	ldq	$25 , 8($30 )
	mov	$24 ,  $at
	negq	$27 ,  $3

	ldq	$23 , 16($30 )
	cmovlt	$at , $3  , $27
	ldq	$3  , 24($30 )

	lda	$30 , 32 ($30 )
	ret	$31 , ($23  ), 1

	.end	__remq
