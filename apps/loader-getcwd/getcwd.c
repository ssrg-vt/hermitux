#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
	char buf[128], print_buf[128];
	getcwd(buf, 128);

	sprintf(print_buf, "cwd: %s\n", buf);
	write(1, print_buf, strlen(print_buf));
	return 0;
}
