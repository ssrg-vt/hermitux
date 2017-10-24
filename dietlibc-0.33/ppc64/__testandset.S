.global __testandset
.type __testandset,@function
.align 2
__testandset:
1:	lwarx	5,0,3
	li	0,1
	stwcx.	0,0,3
	bne-	1b
	mr	3,5
	blr
.size __testandset,.-__testandset

