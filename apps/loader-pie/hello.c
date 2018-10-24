#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define tls_mod_off_t size_t

__thread int x = 4;
extern void *__tls_get_addr(tls_mod_off_t *v);

int main(int argc, char **argv) {
	char str[128];

	//sprintf(str, "get tls addr: %p\n", __tls_get_addr(0));
	sprintf(str, "X: %d\n", x);

	write(1, str, strlen(str));
	printf("printf!\n");

	return 0;
}
