.text
.global htons
.type   htons,@function
.global ntohs
.type   ntohs,@function

htons:
ntohs:
        movzwl  4(%esp),%eax
        xchgb   %al,%ah
        ret
