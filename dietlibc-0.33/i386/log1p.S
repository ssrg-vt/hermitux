.text

.global log1pf,log1p,log1pl
	.type	 log1pf,@function
	.type	 log1p,@function
	.type	 log1pl,@function
log1pf:
	flds 4(%esp)
	jmp __log1p
log1pl:
	fldt 4(%esp)
	jmp __log1p
log1p:
	fldl 4(%esp)
__log1p:
# Sprawdzenie zakresu parametru
	fst %st(1)
	pushl	$0x3ed413cc	# sqrt(2)-1-2^(-25)
	fabs
	flds (%esp)
	popl %eax
	fcompp			# porownanie
	fstsw %ax
	fldln2
	fxch
	sahf
# |x| >= sqrt(2)-1
	jc 1f
	fyl2xp1
	ret
1:	fld1		# x = x + 1
	faddp
	fyl2x
	ret

.ende:
.size    log1p,.ende-log1p
.size    log1pf,.ende-log1pf
.size    log1pl,.ende-log1pl

