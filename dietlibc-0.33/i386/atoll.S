
.text
.type   atoll,@function
.global atoll

atoll:
        movl    4(%esp),%ecx
        push    %edi

        xorl    %edi,%edi               # sign = 0

        decl    %ecx
.Lspaces:
        incl    %ecx                    # while ( isspace(*p) ) p++;
        movb    (%ecx),%al
        cmpb    $' ',%al
        je      .Lspaces
        subb    $9,%al
        cmpb    $5,%al
        jc      .Lspaces                # c = *p - 9;

        cmpb    $'+'-9,%al              # if ( c == '+' - 9 ) p++;
        je      .Lpos
        cmpb    $'-'-9,%al              # else if ( c == '-' - 9 ) sign = -sign, p++;
        jne     .Lnosign
        decl    %edi
.Lpos:  incl    %ecx
.Lnosign:

        push    %ebx
        push    %esi
        push    %ebp
        movl    $10,%ebp

        xorl    %esi,%esi               # Hi(value) = 0;
        xorl    %eax,%eax               # Lo(value) = 0;
.Lcont: movzbl  (%ecx),%ebx             # while ( (unsigned) (*p - '0') < 10 )
        inc     %ecx
        subl    $'0',%ebx
        cmpl    %ebp,%ebx
        jnc     .Lfini
        imull   %ebp,%esi               #     value *= 10,
        mull    %ebp
        addl    %edx,%esi
        addl    %ebx,%eax               #     value += (*p - '0');
        adcl    $0,%esi
        jmp     .Lcont

.Lfini: xchg    %edx,%esi               # return sign== 0 ? +value
        xorl    %edi,%eax               #        sign==-1 ? -value
        xorl    %edi,%edx
        subl    %edi,%eax
        sbbl    %edi,%edx

        pop     %ebp
        pop     %esi
        pop     %ebx
        pop     %edi
        ret

.Lende:

.size    atoll,.Lende-atoll

