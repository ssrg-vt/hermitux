/*
  Copyright (C) 2001, 2002 Thomas M. Ogrisegg

  This is free software. You can redistribute and
  modify it under the terms of the GNU General Public
  Public License.

  This file is part of the hppa-Port of dietlibc

  unified.S
    General system-call interface
*/

#include "dietfeatures.h"

.level 1.1

.text

.export __unified_syscall
.global __unified_syscall
.type __unified_syscall,@function

.export __unified_syscall5
.global __unified_syscall5
.type __unified_syscall5,@function

.export __unified_syscall6
.global __unified_syscall6
.type __unified_syscall6,@function

.export __error_unified_syscall
.global __error_unified_syscall


.globl _exit
.weak exit
_exit:
exit:
        ldi 1, %r20
	b __unified_syscall
	nop

__unified_syscall6:
	ldw -0x38(%sr0, %sp), %r21
__unified_syscall5:
	ldw -0x34(%sr0, %sp), %r22
__unified_syscall:
	be,l 0x100(%sr2, %r0), %sr0, %r31
	nop
	ldi -0x100, %r1
	cmpb,>>=,n %r1, %ret0, .Lnoerr
	sub %r0, %ret0, %ret0
__error_unified_syscall:
#ifdef WANT_THREAD_SAFE
	copy %r2, %r23
	bl __errno_location, %r2
	copy %ret0, %r20
	stw %r20, 0(%sr0, %ret0)
	copy %r23, %r2
#else
	ldil LP%errno, %r26
	ldo  RP%errno(%r26), %r26
	stw  %ret0, 0(%r0, %r26)
#endif
	ldi -1, %ret0
.Lnoerr:
/* here we go and "reuse" the return for weak-void functions */
#include "dietuglyweaks.h"

	bv,n %r0(%rp)
