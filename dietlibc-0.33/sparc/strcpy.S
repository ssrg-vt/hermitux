/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    strcpy.S
      sparc assembler implementation of strcpy(3)
*/

.text
.globl strcpy
strcpy:
	clr %o2
1:
	ldub [%o1+%o2], %o3
	stb  %o3, [%o0+%o2]
	tst  %o3
	bne,a 1b
	inc %o2
	retl
	nop
