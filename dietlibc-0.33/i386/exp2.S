.text
.type   exp2,@function
.global exp2
exp2:                           # note: exp2(x) = 2^x
        fldl     4(%esp)        # x
        fld      %st(0)         # x              x
        frndint                 # int(x)         x
        fxch                    # x              int(x)
        fsub     %st(1),%st(0)  # frac(x)        int(x)
        f2xm1                   # 2^(fract(x))-1 int(x)
        fld1                    # 1              2^(fract(x))-1         int(x)
        faddp    %st(0),%st(1)  # 2^(fract(x))   int(x)
        fscale                  # 2^x            int(x)
        fstp     %st(1)         # 2^x
        ret

.ende:
.size    exp2,.ende-exp2
