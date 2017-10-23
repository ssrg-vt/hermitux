#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>

extern int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg); 

#define SYSCATCH
#define ITERATIONS	10000

inline static unsigned long long rdtsc(void) {
	unsigned long lo, hi;
	asm volatile ("rdtsc" : "=a"(lo), "=d"(hi) :: "memory");
	return ((unsigned long long) hi << 32ULL | (unsigned long long) lo);
}

int main(int argc, char** argv)
{
	int i;
	unsigned long long start, stop;
	struct winsize sz;
	char buf[128];

	start = rdtsc();
	for(i=0; i<ITERATIONS; i++) {
#ifdef SYSCATCH
		ioctl(0, TIOCGWINSZ, &sz);
#else
		sys_ioctl(0, TIOCGWINSZ, (unsigned long)&sz);
#endif /* SYSCATCH */
		(void)sz;
	}
	stop = rdtsc();

	sprintf(buf, "Result: %llu\n", stop - start);
	write(1, buf, strlen(buf));

	return 0;
}
