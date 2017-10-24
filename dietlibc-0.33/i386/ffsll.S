.text
.align 0
.global ffsll
.type	 ffsll,@function
ffsll:
	xorl %eax,%eax
	orl 4(%esp),%eax
	jz .Lsecond
	bsfl %eax,%eax
	incl %eax
.Ldone:
	ret
.Lsecond:
	orl 8(%esp),%eax
	jz .Ldone2
	bsfl %eax,%eax
	addl $33,%eax
.Ldone2:
	ret
.Lende:
.size ffsll,.Lende-ffsll
