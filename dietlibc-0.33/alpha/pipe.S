#include "syscalls.h"

.text
.align 2
.global pipe
.type pipe,@function
pipe:
        lda     $0, __NR_pipe($31)
        callsys
        bne     $19, .Lerror
        stl     $0,  0($16)
        stl     $20, 4($16)
        clr     $0
        ret     $31, ($26), 0x01
.Lerror:
        br      error_unified_syscall

