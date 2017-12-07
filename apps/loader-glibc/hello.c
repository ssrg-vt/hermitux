#include <stdio.h>
#include <stdlib.h>
#include <sys/auxv.h>

int main(int argc, char **argv) {
	
	printf("argv[0] = %s\n", argv[0]);
	printf("AT_BASE: %#lx\n", getauxval(AT_BASE));
	printf("AT_RANDOM: %#lx\n", getauxval(AT_RANDOM));

	return 0;
}
