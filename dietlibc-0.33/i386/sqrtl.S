.text
.type   sqrtl,@function
.global sqrtl
sqrtl:
        fldt   4(%esp)
        fsqrt
        ret

.ende:
.size    sqrtl,.ende-sqrtl

