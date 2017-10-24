.text

.global cosf,cos,cosl
.type   cosf,@function
.type   cos,@function
.type   cosl,@function

cosf:
	flds	4(%esp)
	jmp 1f
cos:
	fldl	4(%esp)
1:
	fcos
	fnstsw	%ax
	testb	$0x04, %ah
	je 3f
	fldpi
	fadd	%st
	fxch	%st(1)
2:	fprem1
	fnstsw	%ax
	testb	$0x04, %ah
	jne	2b
	fstp	%st(1)
        fcos
3:	ret
cosl:
        fldt    4(%esp)
        jmp 1b

.Lende:
.size    cos,.Lende-cos
.size    cosl,.Lende-cosl
.size    cosf,.Lende-cosf
