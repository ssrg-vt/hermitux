#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

extern char **environ;

int main(int argc, char **argv) {
	int i;

	printf("argc: %d\n", argc);
	for(i=0; i<argc; i++) {
		printf("  argv[%d]: %s (@%p)\n", i, argv[i], &(argv[i][0]));
	}

	printf("environ: %x\n", environ);
	i = 0;
	while(environ[i] != NULL) {
		printf("environ[%d] @%p: '%s'\n", i, &(environ[i][0]), environ[i]);
		i++;
	}
	return 0;
}
