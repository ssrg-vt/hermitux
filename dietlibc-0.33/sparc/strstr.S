/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    strstr.S
      sparc assembler implementation of strstr(3)
*/

.text
.globl strstr
strstr:
	ldub [%o1], %o2
.Lloop:
	ldub [%o0], %o3
	tst %o3
	be,a .Lret
	clr %o0
	cmp %o3, %o2
	bne,a .Lloop
	inc %o0
!-------------------------------
	clr %o5
.Lstrcmp:
	ldub [%o0+%o5], %o3
	ldub [%o1+%o5], %g1
	tst %g1
	be .Lret
	cmp %o3, %g1
	bne,a .Lloop
	inc %o0
	b .Lstrcmp
	inc %o5
.Lret:
	retl
	nop
