.text

.global fabsf,fabs,fabsl
	.type	fabsf,@function
	.type	fabs,@function
	.type	fabsl,@function
fabsf:
	flds 4(%esp)
	fabs
	ret
fabs:
	fldl 4(%esp)
	fabs
	ret
fabsl:
	fldt 4(%esp)
	fabs
	ret

.ende:
.size    fabs,.ende-fabs
.size    fabsl,.ende-fabsl
.size    fabsf,.ende-fabsf
