.text
.global __CAS
.type __CAS,@function
__CAS:
	movl 4(%esp), %edx
	movl 8(%esp), %eax
	movl 12(%esp), %ecx
	lock cmpxchgl %ecx, (%edx)
	ret
.size __CAS, .-__CAS
