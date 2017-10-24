#ifndef __I386_PIC_H__
#define __I386_PIC_H__
#ifdef __DYN_LIB

#undef __i686

#if 1
/* don't trash the athlon return stack */
#if (__GNUC__ >= 3)	// FIXME: how do I check for the binutils version ?
.section .gnu.linkonce.t.__i686.get_pc_thunk.bx,"ax",@progbits
.global __i686.get_pc_thunk.bx
.hidden __i686.get_pc_thunk.bx
.type   __i686.get_pc_thunk.bx,@function
__i686.get_pc_thunk.bx:
	movl	(%esp), %ebx
	ret
.previous

.macro PIC_INIT
	call	__i686.get_pc_thunk.bx
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx
.endm
#else
.text
.Lgetpic:
	mov	(%esp),%ebx
	ret

.macro PIC_INIT
	call	.Lgetpic
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx
.endm
#endif
#else
/* standard code for PIC init */
.macro PIC_INIT
	call	0f
0:	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-0b], %ebx
.endm
#endif

#define PIC_SAVE	pushl	%ebx
#define PIC_RESTORE	popl	%ebx


/* get memory variable (var) in register (reg) */
.macro GET_VAR var reg
	movl	\var@GOT(%ebx), \reg
	movl	(\reg), \reg
.endm

/* put register (reg) into memory variable (var) TRASH register (tmp) */
.macro PUT_VAR reg var tmp
	movl	\var@GOT(%ebx), \tmp
	movl	\reg, (\tmp)
.endm

/* get memory variable (var) in register (reg)
 * IF no more PIC-code is needed */
.macro GET_1VAR var reg
	PIC_SAVE
	PIC_INIT
	GET_VAR \var, \reg
	PIC_RESTORE
.endm

/* put register (reg) into memory variable (var)
 * IF no more PIC-code is needed */
.macro PUT_1VAR reg var
	PIC_SAVE
	PIC_INIT
	PUT_VAR \reg, \var, %ebx
	PIC_RESTORE
.endm

#else

#define PIC_SAVE
#define PIC_RESTORE
#define PIC_INIT

.macro GET_VAR var reg
	movl	\var, \reg
.endm

.macro PUT_VAR reg var tmp
	movl	\reg, \var
.endm

.macro GET_1VAR var reg
	GET_VAR \var, \reg
.endm

.macro PUT_1VAR reg var
	PUT_VAR \reg, \var, none
.endm

#endif
#endif
