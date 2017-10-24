
.text

.global asinf,asin,asinl	# ,__asin
	.type	 asinf,@function
	.type	 asin,@function
	.type	 asinl,@function
#	.type	 __asin,@function
asinf:
	flds 4(%esp)
	jmp __asin
asinl:
	fldt 4(%esp)
	jmp __asin
asin:
	fldl 4(%esp)
__asin:
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fpatan
	ret

.ende:
.size    asin,.ende-asin
.size    asinf,.ende-asinf
.size    asinl,.ende-asinl
