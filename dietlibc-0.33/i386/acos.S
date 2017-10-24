
.text

.global acosf,acos,acosl	# ,__acos
	.type	 acosf,@function
	.type	 acos,@function
	.type	 acosl,@function
#	.type	 __acos,@function
acosf:
	flds 4(%esp)
	jmp __acos
acosl:
	fldt 4(%esp)
	jmp __acos
acos:
	fldl 4(%esp)
__acos:
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fxch	%st(1)
	fpatan
	ret

.ende:
.size    acos,.ende-acos
.size    acosf,.ende-acosf
.size    acosl,.ende-acosl
