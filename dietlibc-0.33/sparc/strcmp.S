/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    strcmp.S
      sparc assembler implementation of strcmp(3)
*/

.text
.globl strcmp
strcmp:
	clr %o4
.Lloop:
	ldub [%o0+%o4], %o2
	ldub [%o1+%o4], %o3
	tst %o2
	be .Lret
	cmp %o2, %o3
	be,a .Lloop
	inc %o4
.Lret:
	retl
	sub %o2, %o3, %o0
