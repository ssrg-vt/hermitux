#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
	double a;

	printf("argv[0] = %s\n", argv[0]);
	a = sqrt(4);

	printf("%f\n", a);

	printf("pid: %d\n", getpid());

	return 0;
}
