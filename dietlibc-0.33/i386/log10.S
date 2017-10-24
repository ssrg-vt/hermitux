.text

.global log10f,log10,log10l
	.type	 log10f,@function
	.type	 log10,@function
	.type	 log10l,@function
log10f:
	fldlg2
	flds 4(%esp)
	fyl2x
	ret
log10l:
	fldlg2
	fldt 4(%esp)
	fyl2x
	ret
log10:
	fldlg2
	fldl 4(%esp)
	fyl2x
	ret


.ende:
.size    log10,.ende-log10
.size    log10f,.ende-log10f
.size    log10l,.ende-log10l
