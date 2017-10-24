/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    memcmp.S
      sparc assembler implementation of memcmp(3)
*/

.text
.globl memcmp
memcmp:
	clr %o5
.Lloop:
	subcc %o2, 1, %o2
	bl,a .Lret
	mov %o3, %o4
	ldub [%o0+%o5], %o3
	ldub [%o1+%o5], %o4
	cmp %o3, %o4
	be,a .Lloop
	inc %o5
.Lret:
	retl
	sub %o3, %o4, %o0
