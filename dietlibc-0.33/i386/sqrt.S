.text

.global sqrtf,sqrt,sqrtl
	.type	 sqrtf,@function
	.type	 sqrt,@function
	.type	 sqrtl,@function
sqrtf:
	flds 4(%esp)
	fsqrt
	ret
sqrt:
	fldl 4(%esp)
	fsqrt
	ret
sqrtl:
	fldt 4(%esp)
	fsqrt
	ret

.ende:
.size    sqrt,.ende-sqrt
.size    sqrtf,.ende-sqrtf
.size    sqrtl,.ende-sqrtl
