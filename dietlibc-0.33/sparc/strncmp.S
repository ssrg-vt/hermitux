/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    strncmp.S
      sparc assembler implementation of strncmp(3)
*/

.text
.globl strncmp
strncmp:
	clr %o5
1:
	subcc %o2, 1, %o2
	bl,a .Lret
	add %g0, %o3, %o4
	ldub [%o0+%o5], %o3
	ldub [%o1+%o5], %o4
	cmp %o4, %o3
	bne .Lret
	tst %o4
	bne,a 1b
	inc %o5
.Lret:
	retl
	sub %o3, %o4, %o0
