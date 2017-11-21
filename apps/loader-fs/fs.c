#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TARGET_FILE	"test-file.txt"
#define STR_SIZE	7
#define TARGET_DIR "test-dir"

int rdwr_fd(void);
int fptr(void);
int dir_tests(void);
int fail_tests(void);
int del_tests(void);

int main(void) {
	int ret;

	ret = rdwr_fd();
	if(ret < 0)
		return ret;

	ret = fptr();
	if(ret < 0)
		return ret;

	ret = dir_tests();
	if(ret < 0)
		return ret;

	ret = del_tests();
	if(ret < 0)
		return ret;

	ret = fail_tests();
	if(ret < 0)
		return ret;
}

int fptr(void) {
	FILE *fp;
	int bytes_written, bytes_read, ret;
	char buf[STR_SIZE] = "ghijkl";

	fp = fopen(TARGET_FILE, "rw+");
	if(!fp) {
		perror("fopen");
		printf("huhu\n");
		return -1;
	}

	bytes_written = fwrite(buf, 1, STR_SIZE, fp);
	if(bytes_written != STR_SIZE) {
		perror("write");
		ret = -2;
		goto out_close;
	}

	if(fseek(fp, 0, SEEK_SET) != 0) {
		perror("fseek");
		ret = -4;
		goto out_close;
	}

	bytes_read = fread(buf, 1, STR_SIZE, fp);
	if(bytes_read != STR_SIZE) {
		perror("fread");
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
	if(fd == -1) {
		perror("open");
		return -1;
	}

	bytes_written = write(fd, buf, strlen(buf));
	if (bytes_written != strlen(buf)) {
		perror("write");
		ret = -2;
		goto out_close;
	}

	bzero(buf, STR_SIZE);

	if(lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		ret = -3;
		goto out_close;
	}

	bytes_read = read(fd, buf, STR_SIZE);
	if (bytes_read != STR_SIZE) {
		perror("read");
		ret = -4;
		goto out_close;
	}

	printf("Read: %s\n", buf);

	ret = 0;
out_close:
	close(fd);

/*	ret = unlink(TARGET_FILE);
	if(ret == -1) {
		perror("unlink");
		return -5;
	} */

	return ret;
}

int dir_tests(void) {
	int ret;

	ret = mkdir(TARGET_DIR, 0777);
	if(ret == -1) {
		perror("mkdir");
		return -1;
	}

	ret = rmdir(TARGET_DIR);
	if(ret == -1) {
		perror("rmdir");
		return -2;
	}

	return 0;
}

int fail_tests(void) {
	int fd;
	FILE *fp;

	/* open non existent file */
	fd = open("non-existent-file", O_RDONLY);
	if(fd == -1)
		perror("expected - no such file or directory");

	fp = fopen("non-existent-file", "r");
	if(!fp)
		perror("expected - no such file or directory");

	return 0;
}

int del_tests(void) {
	int fd, ret;

	fd = open(TARGET_FILE, O_RDWR | O_TRUNC | O_CREAT, 0600);
	close(fd);

	ret = unlink(TARGET_FILE);
	if(ret == -1) {
		perror("unlink");
		return -5;
	} 

	return 0;
}
