#define _GNU_SOURCE
#include <sched.h>

extern int __syscall_sched_getaffinity(pid_t, size_t, cpu_set_t*);

int sched_getaffinity(pid_t pid, size_t size, cpu_set_t *mask)
{
	int ret;

	*mask = 0;
	ret = __syscall_sched_getaffinity(pid, size, mask);
	if (ret > 0)
		return 0;
	return ret;
}
