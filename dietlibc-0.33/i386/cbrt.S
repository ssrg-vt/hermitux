.text

tab:    .byte    0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4
        .byte    4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9
        .byte   10,10,11,11,12,12,12,13,13,14,14,14,15,15,15,16

_3:     .long   0x40400000


.global cbrt
cbrt:
        fldl    4(%esp)
        movw    10(%esp),%ax
        movb    %ah,%ch
        andw    $0x7FFF,%ax
        jz      zero                    /* cbrt(0) = 0              */
        pushl   %ebx
        cwd
        movl    $16*3,%ebx              /* AX   3FE0 3FF0 4000 4010 */
        divw    %bx                     /* AX    154  155  155  155 */
                                        /* DX     20    0   10   20 */
        shlw    $5,%ax
        addb    %ch,%ch
        rcrw    %ax                     /* AX   1540 1550 1550 1550 */
        addw    $0x3FF0-0x1550,%ax      /* AX   3FE0 3FF0 3FF0 3FF0 */
        movw    %dx,%bx
        addb    tab(%ebx),%al
        adcb    $0,%ah
        popl    %ebx
        movw    %ax,10(%esp)
        fldl    4(%esp)                 /* y         x        */

        movl    $4,%ecx
lop:
        fld     %st(0)                  /* y         y    x   */
        fmul    %st(0),%st(0)           /* y^2       y    x   */
        fdivr   %st(2),%st(0)           /* x/y^2     y    x   */
        fadd    %st(1),%st(0)           /* x/y^2+y   y    x   */
        faddp                           /* x/y^2+2y  x        */
        fdivs   _3                      /* y'        x        */
        loop    lop

        fstp    %st(1)                  /* y'                 */
zero:
        ret

