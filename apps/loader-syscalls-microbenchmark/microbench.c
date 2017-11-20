#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define ITERATIONS 	1000000
#define BRK_INCR	8
#define MMAP_SIZE	4096

#define START	gettimeofday(&start, NULL)
#define STOP	gettimeofday(&stop, NULL)

struct timeval start, stop, res;

void print_res(char *name) {
	timersub(&stop, &start, &res);
	printf("%s: %ld.%06ld\n", name, res.tv_sec, res.tv_usec);
}

int main(int argc, char **argv) {
	int pid, i;
	struct timeval tv;
	struct winsize sz;
	size_t brk_val;
	char hostname[128];

	/* getpid */
	START;
	for(i=0; i<ITERATIONS; i++)
		pid = getpid();
	STOP;
	print_res("getpid");

	/* gettimeofday */
	START;
	for(i=0; i<ITERATIONS; i++)
		gettimeofday(&tv, NULL);
	STOP;
	print_res("gettimeofday");

	/* ioctl */
	START;
	for(i=0; i<ITERATIONS; i++)
		ioctl(0, TIOCGWINSZ, &sz);
	STOP;
	print_res("ioctl");

	/* brk */
	START;
	brk_val = (size_t)sbrk(0);
	for(i=0; i<ITERATIONS; i++)
		brk((void *)(brk_val + (i*BRK_INCR)));
	STOP;
	print_res("brk");

	/* gethostname */
	START;
	for(i=0; i<ITERATIONS; i++)
		gethostname(hostname, 128);
	STOP;
	print_res("gethostname");

	/* mmap */
	START;
	for(i=0; i<ITERATIONS; i++)
		mmap(0, MMAP_SIZE, PROT_WRITE, MAP_PRIVATE, -1, 0);
	STOP;
	print_res("mmap");


	return 0;
}
