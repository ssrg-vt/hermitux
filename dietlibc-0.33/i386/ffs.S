.text
.align 0
.global ffs,ffsl
.type	 ffs,@function
.type	 ffsl,@function
ffs:
ffsl:
	xorl %eax,%eax
	orl 4(%esp),%eax
	jz .Ldone
	bsfl %eax,%eax
	incl %eax
.Ldone:
	ret
.Lende:
.size ffsl,.Lende-ffsl
.size ffs,.Lende-ffs
