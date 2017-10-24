/*
  Copyright (C) 2002 Thomas M. Ogrisegg

  This is free software. You can redistribute and
  modify it under the terms of the GNU General Public
  Public License.

  This file is part of the IA64-Port of the dietlibc

  start.S
    Implemenation of the _start startup function
*/

#include "dietfeatures.h"

.psr abi64
.psr lsb
.lsb

.text
.globl _start
.proc  _start

_start:
	alloc r2 = ar.pfs,0,0,7,0
	adds out1 = 16, sp    /* base arguments */
	movl gp = @gprel(0f)  /* base offset */
	;;
0:	
	ld8 out0 = [out1], 8  /* load argc and set argv */
	mov r9 = ip
	;;
	shladd out2=out0,3,out1	/* envp = argv+8*argc */
	sub gp = r9, gp       /* subtract program counter */
	;;
	addl r15 = @ltoff(environ#), gp /* offset to environ */
	adds out2 = 8, out2	/* envp += 8 */
	;;
	ld8  r14 = [r15]
	;;
	st8  [r14] = out2           /* store envp in environ */

#ifdef WANT_DYNAMIC
/* FIXME: dl_init parameter ??? */
	br.call.sptk.few rp = _dyn_start
#else
	br.call.sptk.few rp = CALL_IN_STARTCODE
#endif
	;;
	mov r32 = r8                /* store return code */
	br exit                     /* branch to exit */
.endp _start
.size _start, . - _start
