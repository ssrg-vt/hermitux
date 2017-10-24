.text
.global __CAS
.type __CAS,@function
__CAS:
	mb
1:
	ldq_l $0,0($16)
	cmpeq $0,$17,$1
	beq $1,2f
	mov $18,$1
	stq_c $1,0($16)
	beq $1,1b
	mb
2:
	ret
.size __CAS, .-__CAS
