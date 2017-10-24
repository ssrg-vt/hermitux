.text
.type   fmod,@function
.type   fmodf,@function
.type   fmodl,@function
.type   __fmod2pi,@function
.global fmod, fmodl, fmodf, __fmod2pi

__fmod2pi:
	fldpi
	fadd %st(0)
	fxch
	jmp .Lfmod

fmodf:
	flds 8(%esp)		# y
	flds 4(%esp)		# x
	jmp .Lfmod
fmod:
	fldl 12(%esp)
	fldl 4(%esp)
.Lfmod:
	fprem
	fstsw %ax
	sahf
	jp .Lfmod
	fstp %st(1)
	ret

fmodl:
	fldt 16(%esp)
	fldt 4(%esp)
	jmp .Lfmod


.ende:
.size    fmod,.ende-fmod
.size    fmodl,.ende-fmodl
.size    fmodf,.ende-fmodf
