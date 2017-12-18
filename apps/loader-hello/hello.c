#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>

int main(int argc, char **argv) {
	char buf[128];
	int ret;
	char hello[] = "Hello World";
	struct iovec iov = { .iov_base = hello, .iov_len = strlen(hello) };

        ret = printf("%s\n", hello);
	//ret = writev(1, &iov, 1);
	
	sprintf(buf, "ret = %d\n", ret);
	write(1, buf, strlen(buf));
	return 0;
}
