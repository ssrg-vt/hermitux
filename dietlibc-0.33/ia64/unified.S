/*
   Copyright (C) 2002 Thomas M. Ogrisegg

  This is free software. You can redistribute and
  modify it under the terms of the GNU General Public
  Public License.

  This file is part of the ia64-Port of dietlibc

  unified.S
    General system-call interface
*/

.lsb

.text

.globl __unified_syscall
.proc  __unified_syscall
.globl __error_unified_syscall
.proc  __error_unified_syscall
.globl _exit
.proc  _exit

_exit:
	mov r15 = 1025
.endp _exit
.size _exit, . - _exit

__unified_syscall:
	break.i 0x100000
	movl r2=errno
	cmp.eq p6,p0=-1,r10
	;;
__error_unified_syscall:
(p6)    st4 [r2]=r8
(p6)    mov r8=-1

#include "dietuglyweaks.h"

	br.ret.sptk.few rp

.size __unified_syscall, __error_unified_syscall - __unified_syscall
.size __error_unified_syscall, . - __error_unified_syscall
