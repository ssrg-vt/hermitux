
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define ITERATIONS	10000


extern void sys_msleep(int x);

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
		ioctl(0, TIOCGWINSZ, &sz);
		(void)sz;
	}
	stop = rdtsc();

	sprintf(buf, "Result: %llu\n", stop - start);
	write(1, buf, strlen(buf));

	return 0;
}
