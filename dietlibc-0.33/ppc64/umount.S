#include "syscalls.h"

		.text
diet_proto(umount)
.umount:
		li 0, __NR_umount2
		li 4, 0
		b __unified_syscall
