#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
	double a;

	for(int i=0; i<argc; i++)
		printf("argv[%d] = %s\n", i, argv[i]);
	a = sqrt(4);

	printf("%f\n", a);

	printf("pid: %d\n", getpid());

	return 0;
}
