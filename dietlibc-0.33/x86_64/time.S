/* implement time(2) via gettimeofday(2) on x86-64 because gettimeofday
   is a vsyscall (i.e. no actual switch to kernel mode) */
.text
.global time
.type time,@function
time:
	mov	$0xffffffffff600400,%rax
	call	*%rax
	ret
.Lhere:
	.size	 time,.Lhere-time
