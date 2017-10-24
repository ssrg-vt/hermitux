.text
.global __CAS
.type __CAS,@function
__CAS:
.set noreorder
.set noat
	sync
1:
	ll	$2,0($4)
	bne	$2,$5,2f
	move	$1,$6
	sc	$1,0($4)
	beq	$1,$0,1b
	nop
	sync
2:
	j $31
	nop
.size __CAS, .-__CAS
