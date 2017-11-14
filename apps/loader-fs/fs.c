#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TARGET_FILE	"test-file.txt"
#define STR_SIZE	7

int rdwr_fd(void);
int fptr(void);

int main(void) {
	int ret;

	ret = rdwr_fd();
	if(ret < 0)
		return ret;

	ret = fptr();
	if(ret < 0)
		return ret;
}

int fptr(void) {
	FILE *fp;
	int bytes_written, bytes_read, ret;
	char buf[STR_SIZE] = "ghijkl";

	fp = fopen(TARGET_FILE, "rw+");
	if(!fp) {
		printf("Unable to open fp\n");
		return -1;
	}

	bytes_written = fwrite(buf, 1, STR_SIZE, fp);
	if(bytes_written != STR_SIZE) {
		printf("error fwrite: %d\n", bytes_written);
		ret = -2;
		goto out_close;
	}

	if(fseek(fp, 0, SEEK_SET) != 0) {
		printf("error : fseek\n");
		ret = -4;
		goto out_close;
	}

	bytes_read = fread(buf, 1, STR_SIZE, fp);
	if(bytes_read != STR_SIZE) {
		printf("error fread!: %d\n", bytes_read);
		ret = -3;
		goto out_close;
	}
	printf("fread: %s\n", buf);

	ret = 0;

out_close:
	fclose(fp);

	return ret;
}

int rdwr_fd(void) {
	int fd, bytes_written, bytes_read, ret;
	char buf[STR_SIZE] = "abcdef\n";

	fd = open(TARGET_FILE, O_RDWR | O_TRUNC | O_CREAT, 0600);
	if(!fd) {
		printf("Error openning file\n");
		return -1;
	}

	bytes_written = write(fd, buf, strlen(buf));
	if (bytes_written != strlen(buf)) {
		printf("Error writing in file\n");
		ret = -2;
		goto out_close;
	}

	bzero(buf, STR_SIZE);

	if(lseek(fd, 0, SEEK_SET) == -1) {
		printf("Error lseek\n");
		ret = -3;
		goto out_close;
	}

	bytes_read = read(fd, buf, STR_SIZE);
	if (bytes_read != STR_SIZE) {
		printf("Error reading from file (%d)\n", bytes_read);
		ret = -4;
		goto out_close;
	}

	printf("Read: %s\n", buf);

	ret = 0;
out_close:
	close(fd);

	return ret;
}
