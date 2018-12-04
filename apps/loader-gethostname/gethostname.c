#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE		128
#define NEW_HOSTNAME	"undertow"

int main(int argc, char **argv) {

	char buf[BUF_SIZE] = "xx";

	if(gethostname(buf, BUF_SIZE))
		perror("gethostname");
	printf("gethostname: %s\n", buf);

	printf("setting hostname to %s\n", NEW_HOSTNAME);
	sethostname(NEW_HOSTNAME, BUF_SIZE);

	gethostname(buf, BUF_SIZE);
	printf("gethostname: %s\n", buf);

	return 0;
}
