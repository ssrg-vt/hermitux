.text
.globl __isleap
.type    __isleap,@function

__isleap:
        movl    4(%esp),%eax
        testb   $3,%al
        jne     .Lretzero
#if 1
        movb    $100,%cl        /* this works only for 0 a.D. ... 25599 a.C. */
        divb    %cl
        andb    %ah,%ah
#else
        cltd                    /* This works for 2147483648 b.C. ... 2147483647 a.C. */
        movl    $100,%ecx
        divl    %ecx
        andl    %edx,%edx
#endif
        jne     .Lretone
        testb   $3,%al
        jne     .Lretzero
.Lretone: xorl    %eax,%eax
        incl    %eax
        ret

.Lretzero:xorl    %eax,%eax
        ret

