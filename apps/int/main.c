#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

extern void asm_loop(void);

int main(int argc, char **argv) {
	struct timeval start, stop, res;

	printf("Calling asm ...\n")
		;
	gettimeofday(&start, NULL);
	asm_loop();
	gettimeofday(&stop, NULL);

	printf("Done\n");

	timersub(&stop, &start, &res);
	printf("Asm loop took: %ld.%06ld\n", res.tv_sec, res.tv_usec);

	return 0;
}
