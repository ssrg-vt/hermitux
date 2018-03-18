#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The followign will use brk */
// #define SIZE	(128)

/* The following will rather use mmap */
#define SIZE	(1024*1024*1024)

int main(void) {
	char *ptr;
	int i;

	ptr = malloc(SIZE);
	if(!ptr) {
		printf("malloc failed!\n");
		return -1;
	}

	strcpy(ptr, "hello!");

	printf("content: %s\n", ptr);

	for(i=0; i<SIZE; i++)
		ptr[i] = 'x';

	free(ptr);

	return 0;
}
