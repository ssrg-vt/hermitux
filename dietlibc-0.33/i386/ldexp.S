.text
.global ldexpf,ldexp,ldexpl
	.type ldexpf,@function
	.type ldexp,@function
	.type ldexpl,@function

ldexpf:
	fildl 8(%esp)
	flds 4(%esp)
	fscale
	ret

ldexp:
	fildl 12(%esp)
	fldl 4(%esp)
	fscale
	ret

ldexpl:
	fildl 16(%esp)
	fldt 4(%esp)
	fscale
	ret

.ende:
.size    ldexpl,.ende-ldexpl
.size    ldexp,ldexpl-ldexp
.size    ldexpf,ldexp-ldexpf
