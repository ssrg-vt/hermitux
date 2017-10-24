.text
.global __CAS
.type __CAS,@function
__CAS:
	.mii
	mov ar.ccv = r33
	nop 0
	;;
	nop 0
	.mmb
	cmpxchg8.rel r8 = [r32], r34, ar.ccv
	mf
	br.ret.sptk.many b0
.size __CAS, .-__CAS
