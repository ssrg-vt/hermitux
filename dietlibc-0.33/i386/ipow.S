#
# This is not standard, but often you only need such this function
# which is much shorter than the generic pow() function.
#
#   double  ipow ( double mant, int expo );
#

.text
.global ipow
.type   ipow,@function

ipow:   fld1
        movl    12(%esp),%ecx
        fldl    4(%esp)
        and     %ecx,%ecx
        jns     .Lstart
        negl    %ecx
        fdivr   %st(1),%st(0)
        jmp     .Lstart

.Lnext: fmul    %st(0),%st(0)
.Lstart:shrl    %ecx
        jnc     .Lnomul
        fmul    %st(0),%st(1)
.Lnomul:jnz     .Lnext
        fcomp
        ret
