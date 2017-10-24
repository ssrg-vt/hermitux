/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    memcpy.S
      sparc assembler implementation of memcpy(3)
*/

.text
.globl memcpy
memcpy:
	mov -1, %o3
1:
	subcc %o2, 1, %o2
	bl .Lret
	inc %o3
	ldub [%o1+%o3], %o4
	b 1b
	stb %o4, [%o0+%o3]
.Lret:
	retl
	nop
