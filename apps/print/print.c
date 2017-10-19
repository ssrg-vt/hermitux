#include <stdio.h>
#include <stdlib.h>
#include <hermit/syscall.h>
#include <unistd.h>
#include <sys/uio.h>

int main(void) {
	
	printf("hello\n");

#if 0
	char str1[] = "hello";
	char str2[] = "\n";
	struct iovec iovs[2] = {
		{ .iov_base = &str1, .iov_len = 5 },
		{ .iov_base = &str2, .iov_len = 1 }
	};

	writev(1, iovs, 2);
#endif

	return 0;
}
