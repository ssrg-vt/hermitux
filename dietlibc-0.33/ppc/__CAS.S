.text
.global __CAS
.type __CAS, @function
__CAS:
	mr 9,3
	lwsync
1:
	lwarx 3,0,9
	cmpw 0,3,4
	bnelr- 0
	stwcx. 5,0,9
	bne- 0,1b
	isync
	blr
.size __CAS, .-__CAS
