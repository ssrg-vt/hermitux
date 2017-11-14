#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char **argv) {
	int i;

	printf("argc: %d\n", argc);
	for(i=0; i<argc; i++) {
		printf("  argv[%d]: %s\n", i, argv[i]);
	}

	printf("environ: %x\n", environ);
	i = 0;
	while(environ[i] != NULL)
		printf("%s\n", environ[i++]);

	return 0;
}
