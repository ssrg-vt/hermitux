#include "syscalls.h"
#include <errno.h>

.text
.align 2
.weak clone
clone:
.global __clone
.type __clone,@function
__clone:
	ldiq	$0, EINVAL
	beq	$16, .Lerror
	beq	$17, .Lerror

	subq	$17, 16, $17
	stq	$16, 0($17)
	stq	$19, 8($17)
	mov	$18, $16

	lda	$0, __NR_clone($31)
	callsys
	bne	$19, .Lerror

	beq	$0, .Lstart_thread

	ret	$31, ($26), 0x01

.Lerror:
	jmp	error_unified_syscall

.Lstart_thread:
	clr	$fp

	ldq	$27, 0($sp)
	ldq	$16, 8($sp)

	jsr	$26, ($27), 0x04

	ldgp	$gp, 0($26)
	mov	$0,  $16
	jsr	$26, exit

