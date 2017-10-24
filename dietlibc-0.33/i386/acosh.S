.text
.global acosh
.type   acosh,@function

acosh:
        fldln2
        fldl    4(%esp)
        fld     %st(0)
        fmul    %st(0),%st(0)
        fld1
        fsubrp  %st(0),%st(1)
        fsqrt
        faddp   %st(0),%st(1)
        fyl2x
        ret

.Lende:
        .size    acosh,.Lende-acosh
