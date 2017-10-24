.text
.global __CAS
.type __CAS,@function
__CAS:
	movq %rsi, %rax
	lock cmpxchgq %rdx, (%rdi)
	ret
.size __CAS, .-__CAS
