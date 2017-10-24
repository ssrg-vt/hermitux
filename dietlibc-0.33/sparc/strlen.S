.text
.globl strlen
strlen:
    add  %o0, 1, %o1
.Lloop:
    ldsb [%o0], %o2
    orcc %o2, %o2, %g0
    bne  .Lloop
    add  %o0, 1, %o0
    retl
    sub  %o0, %o1, %o0
