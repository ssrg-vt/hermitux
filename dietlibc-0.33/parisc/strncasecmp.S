/* strncasecmp -- strncasecmp(3) -- compare two strings caseindependently

  Copyright (C) 2002 Thomas M. Ogrisegg

  Created:              09/22/02
  Last updated:         09/23/02

  Remarks:
	none
*/

.text
.globl strncasecmp

strncasecmp:
	.PROC
	.CALLINFO
	.ENTRY
	ldi 0x7a, %r23
	ldi 0x41, %r22
	ldi 0x20, %r21
.Lstrncasecmp:
	addibt,<,n -1, %r24, .Lret
	ldb,ma 1(%r26), %r19
	ldb,ma 1(%r25), %r20
	cmpb,>,n %r22, %r20, .Lno_or
	cmpb,<,n %r23, %r20, .Lno_or
	or %r21, %r19, %r19
	or %r21, %r20, %r20
.Lno_or:
	comclr,= %r0, %r19, %r0
	cmpb,=,n %r19, %r20, .Lstrncasecmp
	nop
.Lret:
	bv %r0(%rp)
	sub %r19, %r20, %r28
	.EXIT
	.PROCEND
	.EXPORT strncasecmp
