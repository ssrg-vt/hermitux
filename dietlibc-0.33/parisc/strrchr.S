/* strrchr -- strrchr(3) -- search last occurence of a character in a string

  Copyright (C) 2002 Thomas M. Ogrisegg

  Created:              09/22/02
  Last updated:         09/23/02

  Remarks:
      none
*/
.text
.globl strrchr

strrchr:
	.PROC
	.CALLINFO
	.ENTRY
	ldi 1, %ret0
.L3:
	ldb,ma 1(%r26), %r19
	comclr,<> %r25, %r19, %r0
	copy %r26, %ret0
	cmpb,<>,n %r0, %r19, .L3
	nop
.Lret:
	bv %r0(%rp)
	addi -1, %ret0, %ret0
	.EXIT
	.PROCEND
	.EXPORT strrchr
