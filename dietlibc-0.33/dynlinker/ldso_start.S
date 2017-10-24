#include <endian.h>

.text
.globl ldso_start
ldso_start:

#ifdef __i386__

	movl %esp, %ebp			/* save stack pointer */
	movl (%ebp), %ecx		/* %ecx = argc */
	leal 4(%ebp), %esi		/* %esi = argv */
	leal 4(%esi,%ecx,4), %eax	/* eax = esi+4*ecx+4 = envp */

	/* call ldso main */
	pushl %eax
	pushl %esi
	pushl %ecx
	movl %eax, environ
	call main
	movl %ebp, %esp		/* restore stack */
	push %eax		/* push start entry of program */

	xorl %eax,%eax		/* clear registers */
	xorl %ebx,%ebx		/* as if kernel starts s static program */
	xorl %ecx,%ecx
	xorl %esi,%esi
	xorl %edi,%edi
	xorl %ebp,%ebp

	movl fini_entry, %edx	/* the dynamic fini for all *.so loaded */
	ret			/* jump to entry point of program */

#endif

#ifdef __arm__

	mov	r4, sp
	mov	fp, #0			/* clear the frame pointer */
	ldr	a1, [sp], #4		/* argc */
	mov	a2, sp			/* argv */
	ldr	ip, .L0
	add	a3, a2, a1, lsl #2	/* &argv[argc] */
	add	a3, a3, #4		/* envp */
	str	a3, [ip, #0]		/* environ = envp */
	bl	main
	mov	sp, r4			/* restore stack */
	mov	lr, a1
	ldr	a1, [pc, #.L1-(.+8)]	/* the dynamic fini for all *.so loaded */
	mov	pc, lr			/* jump to entry point of program */

.align 2
.L0:
	.word environ
.L1:
	.word fini_entry

#endif

/* common data */
.data

.section ".bss"
.align 8

.global errno
errno:
	.long 0

.global environ
environ:
.global __environ
__environ:
#if __WORDSIZE == 64
	.quad 0
#else
	.long 0
#endif

.global environ
environ:
.global __environ
__environ:
#if __WORDSIZE == 64
	.quad 0
#else
	.long 0
#endif

.global fini_entry
fini_entry:
	.long 0
