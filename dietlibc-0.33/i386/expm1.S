.text
.type   expm1,@function
.global expm1
expm1:                          # note: exp(x) = 2^(x*log2(e))
        fldl2e
        fmull    4(%esp)        # x*log2(e)
        fld      %st(0)         # x*log2(e)                       x*log2(e)
        frndint                 # int(x*log2(e))                  x*log2(e)
        fxch                    # x*log2(e)                       int(x*log2(e))
        fsub     %st(1),%st(0)  # frac(x*log2(e))                 int(x*log2(e))
        f2xm1                   # 2^(fract(x*log2(e)))-1          int(x*log2(e))
        fscale                  # 2^(x*log2(e))-2^int(x*log2(e))  int(x*log2(e))
        fxch                    # int(x*log2(e))                  2^(x*log2(e))-2^int(x*log2(e))
        fld1                    # 1                               int(x*log2(e))                  2^(x*log2(e))-2^int(x*log2(e))
        fscale                  # 2^int(x*log2(e))                int(x*log2(e))                  2^(x*log2(e))-2^int(x*log2(e))
        fstp     %st(1)         # 2^int(x*log2(e))                2^(x*log2(e))-2^int(x*log2(e))
        fld1                    # 1                               2^int(x*log2(e))                2^(x*log2(e))-2^int(x*log2(e))
        fsubrp   %st(1)         # 2^int(x*log2(e))-1              2^(x*log2(e))-2^int(x*log2(e))
        faddp    %st(1)
        ret

.ende:
.size    expm1,.ende-expm1
