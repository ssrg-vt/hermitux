#include "dietfeatures.h"

#define zero 0
#define ra 31
#define sp 29
#define a0 4
#define a1 5
#define a2 6
#define gp 28

.text
.global __start
.type __start,@function
__start:
#ifdef __pic__
/* initialize gp pointer */
	.set noreorder
	bltzal $0,0f
	nop
0:	.cpload	$31
	.set reorder
#else
	/* _gp is provided by the linker and points into the middle of the
	 * "small data" section (.sdata and .sbss) allocated by the
	 * -G 8 compiler option */
	la      $gp, _gp
#endif
	move	$ra, $zero	/* prime stack frame */
	lw	$a0, 0($sp)	/* load argc */
	addu	$a1, $sp, 4	/* load argv.  huh?  should be 4, right? */

	and	$sp, 0xfffffff8	/* align stack to 8 bytes */
	subu	$sp, 24		/* make room for 4 arguments, RA + pad */
	/* I don't understand the MIPS calling convention.  Why do you
	   need to make room on the stack for arguments you pass in
	   registers?  Anyway, if we don't do this, the arguments are
	   garbled. */
	sw	$ra, 20($sp)	/* close stack frame */

	addu	$a2, $a0, 1	/* load envp */
	sll	$a2, $a2, 2
#ifdef WANT_DYNAMIC
/* FIXME: dl_init parameter ??? */
	la	$25, _dyn_start
#else
	la	$25, CALL_IN_STARTCODE
#endif
	add	$a2, $a2, $a1
	sw	$a2, environ
	jalr	$25
	la	$25, exit
	move	$4,$2
	jalr	$25

.global __main
.type __main,@function
__main:
	j	$31
