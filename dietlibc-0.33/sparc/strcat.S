/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    strcat.S
      sparc assembler implementation of strcat(3)
*/

.text
.globl strcat
strcat:
	mov %o0, %o4
1:
	ldub [%o0], %o2
	tst %o2
	bne,a 1b
	inc %o0
	clr %o3
2:
	ldub [%o1+%o3], %o2
	stb %o2, [%o0+%o3]
	tst %o2
	bne,a 2b
	inc %o3
	retl
	mov %o4, %o0
