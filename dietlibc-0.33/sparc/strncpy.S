/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    strncpy.S
      sparc assembler implementation of strncpy(3)
*/

.text
.globl strncpy
strncpy:
	clr %o4
.Lloop:
	subcc %o2, 1, %o2
	bl .Lret
	nop                 ! "dst" may be NULL
	ldub [%o1+%o4], %o3
	stb %o3, [%o0+%o4]
	tst %o3
	bne,a .Lloop
	inc %o4
.Lret:
	retl
	nop
