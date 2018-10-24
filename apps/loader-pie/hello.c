#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

__thread int x = 4;

int main(int argc, char **argv) {

	printf("Hello, world!\n");
	printf("tls x %d\n", x);

	return 0;
}
