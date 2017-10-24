
.text
.type   exp10,@function
.global exp10
.type   pow10,@function
.global pow10


pow10:
exp10:                          # note: 10^(x) = 2^(x*log2(10))
        fldl2t
        fmull    4(%esp)        # x*log2(10)
        fld      %st(0)         # x*log2(10)              x*log2(10)
        frndint                 # int(x*log2(10))         x*log2(10)
        fxch                    # x*log2(10)              int(x*log2(10))
        fsub     %st(1),%st(0)  # frac(x*log2(10))        int(x*log2(10))
        f2xm1                   # 2^(fract(x*log2(10)))-1 int(x*log2(10))
        fld1                    # 1                       2^(fract(x*log2(10)))-1       int(x*log2(10))
        faddp    %st(0),%st(1)  # 2^(fract(x*log2(10)))   int(x*log2(10))
        fscale                  # 2^(x*log2(10))          int(x*log2(10))
        fstp     %st(1)         # 2^(x*log2(10))
        ret

.ende:

.size    exp10,.ende-exp10
.size    pow10,.ende-pow10
