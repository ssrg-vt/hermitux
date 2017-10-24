.text
.global atan2f,atan2,atan2l
	.type	 atan2f,@function
	.type	 atan2,@function
	.type	 atan2l,@function
atan2f:
	flds 4(%esp)
	flds 8(%esp)
	fpatan
	ret
atan2l:
	fldt 4(%esp)
	fldt 16(%esp)
	fpatan
	ret
atan2:
	fldl 4(%esp)
	fldl 12(%esp)
	fpatan
	ret

.ende:
.size    atan2,.ende-atan2
.size    atan2f,.ende-atan2f
.size    atan2l,.ende-atan2l
