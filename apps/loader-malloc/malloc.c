#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE	(128)

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
