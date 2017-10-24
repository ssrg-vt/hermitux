.text
.global __CAS
.type __CAS,@function
__CAS:
	membar 15
	jmp %o7+8
	casx [%o0], %o1, %o2
.size __CAS, .-__CAS
