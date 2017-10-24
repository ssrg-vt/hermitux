/*
  Copyright (C) 2001, 2002 Thomas M. Ogrisegg

  This is free  software. You can  redistribute  and
  modify it under the terms of the GNU General Public 
  License.

  This file is part of the hppa-Port of the dietlibc

  setjmp.S
    setjmp(3) implementation
*/

#include <setjmp.h>
.level 1.1

.text

.weak setjmp
.type setjmp,@function
.globl _setjmp
.type _setjmp,@function
.globl __setjmp
.type __setjmp,@function
setjmp:
_setjmp:
__setjmp:
	ldi 0, %r25

.globl __sigsetjmp
.type __sigsetjmp,@function
__sigsetjmp:
	/* Save GR registers */
	stw %r3,  0x00(%sr0, %r26)
	stw %r4,  0x08(%sr0, %r26)
	stw %r5,  0x0c(%sr0, %r26)
	stw %r6,  0x10(%sr0, %r26)
	stw %r7,  0x14(%sr0, %r26)
	stw %r8,  0x18(%sr0, %r26)
	stw %r9,  0x1c(%sr0, %r26)
	stw %r10, 0x20(%sr0, %r26)
	stw %r11, 0x24(%sr0, %r26)
	stw %r12, 0x28(%sr0, %r26)
	stw %r13, 0x2c(%sr0, %r26)
	stw %r14, 0x30(%sr0, %r26)
	stw %r15, 0x34(%sr0, %r26)
	stw %r16, 0x38(%sr0, %r26)
	stw %r17, 0x3c(%sr0, %r26)
	stw %r18, 0x40(%sr0, %r26)
	stw %r19, 0x44(%sr0, %r26)
	stw %dp,  0x48(%sr0, %r26)
	stw %sp,  0x4c(%sr0, %r26)
	stw %rp,  0x50(%sr0, %r26)
	/* Floating point stack in %r19 */
	ldo 0x58(%r26), %r19
	/* Save Floating point registers */
	fstd,ma %fr12, 8(%sr0, %r19)
	fstd,ma %fr13, 8(%sr0, %r19)
	fstd,ma %fr15, 8(%sr0, %r19)
	fstd,ma %fr16, 8(%sr0, %r19)
	fstd,ma %fr17, 8(%sr0, %r19)
	fstd,ma %fr18, 8(%sr0, %r19)
	fstd,ma %fr19, 8(%sr0, %r19)
	fstd,ma %fr20, 8(%sr0, %r19)
	fstd %fr21, 0(%sr0, %r19)
	b,l __sigjmp_save, %r0
	nop
