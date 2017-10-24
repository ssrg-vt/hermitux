.text
.global atanh
.type   atanh,@function

atanh:
        fldln2
        fldl    4(%esp)
        fld1
        fld     %st(1)
        fadd    %st(1),%st(0)
        fxch    %st(1)
        fsubp   %st(0),%st(2)
        fdivp   %st(0),%st(1)
        fyl2x
        fmuls   __half
        ret

.Lende:
        .size    atanh,.Lende-atanh
