.text

.global logf,log,logl
	.type	 logf,@function
	.type	 log,@function
	.type	 logl,@function
logf:
	fldln2
	flds 4(%esp)
	fyl2x
	ret
logl:
	fldln2
	fldt 4(%esp)
	fyl2x
	ret
log:
	fldln2
	fldl 4(%esp)
	fyl2x
	ret

.Lende:
.size    log,.Lende-log
.size    logl,.Lende-logl
.size    logf,.Lende-logf
