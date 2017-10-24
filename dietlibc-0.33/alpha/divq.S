/* taken from glibc 2.2 */

        .set noreorder
        .set noat

        .ent __divqu
        .globl __divqu

        .align 3
__divqu :
        lda     $30 , - 48 ($30 )
        .frame  $30 , 48 , $23  , 0
        .prologue 0
$udiv:
        stq     $1 , 0($30 )
        mov  $25   ,  $1
        stq     $2 , 8($30 )
        mov  $24   ,  $2
        stq     $0 , 16($30 )
        clr     $27
        stq     $3  , 24($30 )
        ldiq    $0  , 1
         stq $4   ,32($30 )
        beq     $1  , $divbyzero

        .align 3


1:      cmpult  $1  , $2  , $at
        blt     $1  , 2f
        addq    $1  , $1  , $1
        addq    $0  , $0  , $0
        bne     $at  , 1b
        unop
2:
3:       addq $27    ,$0  , $4
        srl     $0  , 1, $0
        cmpule  $1  , $2  , $at
        subq    $2  , $1  , $3
         cmovne $at   ,$4  , $27
        srl     $1  , 1, $1
        cmovne  $at  , $3  , $2
        bne     $0  , 3b

$done:  ldq     $1 , 0($30 )
        ldq     $2 , 8($30 )
        ldq     $0 , 16($30 )
        ldq     $3  , 24($30 )
         ldq $4   ,32($30 )
        lda     $30 , 48 ($30 )
        ret     $31 , ($23  ), 1

$divbyzero:
        mov     $16 , $3
        ldiq    $16 , -2
        call_pal 170
        mov     $3  , $16
        clr     $27
        br      $done

        .end __divqu

        .ent __divq
        .globl __divq

        .align 3
__divq :
        lda     $30 , - 48 ($30 )
        .frame  $30 , 48 , $23  , 0
        .prologue 0
        or      $24  , $25  , $at

        bge     $at , $udiv


        stq     $24  , 0($30 )
        negq  $24   ,  $at
        stq     $25  , 8($30 )
        cmovge  $at , $at , $24
        stq     $23  , 16($30 )
        negq  $25   ,  $at
        stq     $3  , 24($30 )
        cmovge  $at , $at , $25


        bsr     $23  , __divqu


        ldq     $24  , 0($30 )
        ldq     $25  , 8($30 )
        xor $24  , $25  ,  $at
        negq  $27   ,  $3

        ldq     $23  , 16($30 )
        cmovlt  $at , $3  , $27
        ldq     $3  , 24($30 )

        lda     $30 , 48 ($30 )
        ret     $31 , ($23  ), 1

        .end    __divq

