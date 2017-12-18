#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SYSCATCH
#define ITERATIONS	1000000

inline static unsigned long long rdtsc(void) {
	unsigned long lo, hi;
	asm volatile ("rdtsc" : "=a"(lo), "=d"(hi) :: "memory");
	return ((unsigned long long) hi << 32ULL | (unsigned long long) lo);
}

int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg); 

int main(int argc, char** argv)
{
	int i, ret;
	unsigned long long start, stop;
	struct winsize sz;
	char buf[128];
	FILE *res_file;

	if (argc != 3)
	{
		return -1;
	}

	if (!strcmp(argv[1], "func")) {
		start = rdtsc();
		for(i=0; i<ITERATIONS; i++) {
			ret = sys_ioctl(0, TIOCGWINSZ, (unsigned long)&sz);
			(void)sz;
		}
		stop = rdtsc();
	}
	else if (!strcmp(argv[1], "syscall")) {
		start = rdtsc();
		for(i=0; i<ITERATIONS; i++) {
			ret = ioctl(0, TIOCGWINSZ, &sz);
			(void)sz;
		}
		stop = rdtsc();
	}
	res_file = fopen(argv[2], "a");
	fprintf(res_file, "%llu\n", stop - start);
	fclose(res_file);

	/* write(1, buf, strlen(buf)); */
	/* printf(buf, "Return: %d\n", ret); */
	/* write(1, buf, strlen(buf)) */

	return 0;
}

int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg) {
	/* Check cmd, we want that to fail on commands that we did not explore */
	switch(cmd) {
	case TIOCGWINSZ:
	{
		struct winsize *res = (struct winsize *)arg;
		/* Quick hack, FIXME */
		res->ws_row = 24;
		res->ws_col = 80;
		res->ws_xpixel = 0;
		res->ws_ypixel = 0;
		return 0;
	}

	default:
		printf("unsupported ioctl command 0x%x\n", cmd);
		return -ENOSYS;
	}	

	/* should not come here */
	return -1;
}
