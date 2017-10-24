#include <setjmp.h>

.text
.global __longjmp
.type __longjmp,@function
__longjmp:
	lm	%r6,%r15,0(%r2)
	ld	%f6,48(%r2)
	ld	%f4,40(%r2)
	chi	%r3,0
	jne	1f
	lhi	%r3,1
1:	lr	%r2,%r3
	br	%r14
.size __longjmp,.-__longjmp;
