#include <stdio.h>
#include <stdint.h>

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

	GET_RSP(rsp);
	for(i=0; i<PRINT_STACK_SIZE; i+=4) {
		uint32_t val = *(rsp+i);
		printf("0x%lx: 0x%x\n ", (rsp+i), val);
	}

	return 0;
}
