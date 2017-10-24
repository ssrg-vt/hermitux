
.text

.global atanf,atan,atanl
	.type	 atanf,@function
	.type	 atan,@function
	.type	 atanl,@function
atanf:
	flds 4(%esp)
	fld1
	fpatan
	ret
atan:
	fldl 4(%esp)
	fld1
	fpatan
	ret
atanl:
	fldt 4(%esp)
	fld1
	fpatan
	ret

.ende:
.size    atan,.ende-atan
.size    atanf,.ende-atanf
.size    atanl,.ende-atanl
