.text

.global rintf,rint,rintl
	.type	 rintf,@function
	.type	 rint,@function
	.type	 rintl,@function
rintf:
	flds 4(%esp)
	frndint
	ret
rint:
	fldl 4(%esp)
	frndint
	ret
rintl:
	fldt 4(%esp)
	frndint
	ret

.Lende:
.size    rint,.Lende-rint
.size    rintl,.Lende-rintl
.size    rintf,.Lende-rintf
