.text

.global hypotf,hypot,hypotl	# ,__hypot
	.type	 hypotf,@function
	.type	 hypot,@function
	.type	 hypotl,@function
#	.type	 __hypot,@function
hypotf:
	flds 8(%esp)
	flds 4(%esp)
	jmp __hypot
hypotl:
	fldt 16(%esp)
	fldt 4(%esp)
	jmp __hypot
hypot:
	fldl 12(%esp)
	fldl 4(%esp)
__hypot:
	fmul %st(0),%st(0)
	fxch
	fmul %st(0),%st(0)
	faddp
	fsqrt
	ret

.ende:
.size    hypot,.ende-hypot
.size    hypotf,.ende-hypotf
.size    hypotl,.ende-hypotl
