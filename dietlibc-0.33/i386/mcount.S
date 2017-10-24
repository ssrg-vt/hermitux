/*
   Copyright (C) 2001, 2002 Thomas M. Ogrisegg

   This is free software. You can redistribute and
   modify it under the terms of the GNU General Public
   Public License.

   mcount.S
     i386 assembler implementation of mcount
*/

/* .section ".profile" */
.text
.globl mcount
.type  mcount,@function
mcount:
/*
   save all generic registers which
   might be used by __mcount, but aren't
   automatically saved
 */
	pushl %eax
	pushl %ecx
	pushl %edx

/*  push the instruction pointer of the calling function */
	pushl 0xc(%esp)

/*  push the instruction pointer of the
    function that called the calling function */
	pushl 0x4(%ebp)

	call __mcount

/*  clean up stack */
	addl $0x8, %esp

/*  restore the previously saved registers */
	popl %edx
	popl %ecx
	popl %eax
	ret

.size mcount,.-mcount
