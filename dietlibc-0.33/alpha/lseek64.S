#include "syscalls.h"

.global lseek64
lseek64:
	br	lseek
