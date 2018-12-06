#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "eval.h"

#define ITERATIONS 100000
#define DUMMY_SYSCALL_NUMBER 346
#define EXPECTED_RETVAL 2468


static inline long long rdtsc(void)
{
	unsigned long lo, hi;
	asm volatile("rdtsc"
				 : "=a"(lo), "=d"(hi)::"memory");
	return ((unsigned long long)hi << 32ULL | (unsigned long long)lo);
}

int main(int argc, char **argv)
{
	volatile int i, ret;
	unsigned long long sc_start, sc_end;
	FILE *res_file;

	if (argc != 2)
	{
		return -1;
	}

	res_file = fopen(argv[1], "a");

	sc_start = rdtsc();
	for (i = 0; i < ITERATIONS; i++)
	{
		asm volatile("mov $0x15a,%eax\n"
			     "syscall\n"
			);

		/* ret = syscall(DUMMY_SYSCALL_NUMBER); */

		/* if (ret != EXPECTED_RETVAL) */
		/* { */
		/* 	printf("%d is not the expected return value\n", ret); */
		/* 	exit(-1); */
		/* } */
	}
	sc_end = rdtsc();

	fprintf(res_file, "%llu\n", sc_end - sc_start);
	fclose(res_file);

	return 0;
}
