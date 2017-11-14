#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

#define GET_REG( var, reg, size ) asm volatile("mov"size" %%"reg", %0" : "=m" (var) )
#define GET_REG64( var, reg ) GET_REG( var, reg, "q")
#define GET_RSP( var ) GET_REG64( var, "rsp" )

#define PRINT_STACK_SIZE	0

int main(int argc, char **argv) {
	int i;
	char *rsp;

	printf("hello world from linux binary !!\n");
	printf("argc:%d\n", argc);

	for(i=0; i<argc; i++)
		printf("argv[%d] == %s\n", i, argv[i]);

	printf("user env variable: %s\n", getenv("USER"));
	printf("environ[0]: %s\n", environ[0]);
	printf("environ[1]: %s\n", environ[1]);
	printf("environ[2]: %s\n", environ[2]);

	return 0;
}
