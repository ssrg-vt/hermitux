/*
  Copyright (C) 2001, 2002 Thomas M. Ogrisegg

  This is free software. You can redistribute and
  modify it under the terms of the GNU General Public
  Public License.

  This file is part of the hppa-Port of the dietlibc

  start.S
    Implemenation of the _start startup function
*/

#include "dietfeatures.h"

.level 1.1

.text

.export _start
.globl _start
.type _start,@function

_start:
	.PROC
	.CALLINFO CALLS, SAVE_RP
	.ENTRY
	copy %r25, %r26				; %r26 = argc
	zdep %r26, 29, 30, %r19		; %r19 = argc*4
	ldi 4, %r20
	add %r20, %r19, %r19		; %r19 = argc*4+4
	copy %r24, %r25
	add %r19, %r24, %r24
	ldil LP%environ, %r19
	ldo RP%environ(%r19), %r19

	/* Expand the stack to store the 5th through 7th args */
	ldo 64(%sp), %sp

	stw %r24, 0(%sr0, %r19)
	ldil L%$global$, %dp
#ifdef WANT_DYNAMIC
/* FIXME: dl_init parameter ??? */
	bl _dyn_start, %rp
#else
	bl CALL_IN_STARTCODE, %rp
#endif
	ldo R%$global$(%dp), %dp
	b _exit
	or %r28, %r28, %r26
	.EXIT
	.PROCEND
