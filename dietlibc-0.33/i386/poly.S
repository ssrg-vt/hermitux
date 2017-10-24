
.text
.global __poly
.type	__poly,@function


#
#  double  __poly ( double x, int n, const double* c );
#

__poly:
	movl 	16(%esp),%eax		
	movl	12(%esp),%ecx		 
	leal 	(%eax,%ecx,8),%eax
	fldl	 4(%esp)
	fldz
.Lloop:
	fmul 	%st(1),%st(0)
	faddl	(%eax)
	addl 	$-8,%eax
	decl	%ecx
	jns 	.Lloop
	fstp 	%st(1)
	ret
