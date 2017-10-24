#include "syscalls.h"

#ifdef __NR_arm_fadvise64_64
syscall(arm_fadvise64_64, __arm_fadvise64_64);
#endif
