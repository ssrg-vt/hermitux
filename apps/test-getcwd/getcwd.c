#include <stdio.h>
#include <unistd.h>

int main(void) {
	char buf[128];
	getcwd(buf, 128);

	printf("cwd: %s\n", buf);

	return 0;
}
