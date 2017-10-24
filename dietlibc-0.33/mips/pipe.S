#include "syscalls.h"

#define v0      2      /* return value */
#define v1      3      /* return value */
#define a0      4      /* argument registers */
#define a3      7
#define ra      31     /* return address */

	.text
	.align	2
	.globl	pipe
	.ent	pipe
pipe:
	li	$v0,__NR_pipe
	syscall
	bnez	$a3,.Lerror
	sw	$v0,0($a0)
	sw	$v1,4($a0)
	move	$v0,$0
	jr      $ra
.Lerror:
	la	$25, error_unified_syscall
	jr	$25
.end pipe
