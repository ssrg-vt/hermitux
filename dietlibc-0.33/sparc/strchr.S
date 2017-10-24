/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    strchr.S
      sparc assembler implementation of strchr(3)
*/

.text
.globl strchr
strchr:
	ldub [%o0], %o2
	cmp %o2, %o1
	be .Lfound
	tst %o2
	bne,a strchr
	inc %o0
	clr %o0
.Lfound:
	retl
	nop
