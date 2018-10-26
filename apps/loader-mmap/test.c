#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE_SIZE		4096
#define MAP_LEN			(PAGE_SIZE*10)
#define REQUESTED_ADDR	0x200aed000

#define FILENAME		"test.dat"
#define FILELEN			MAP_LEN

void access_memory(void *addr, size_t len) {
	char *ptr = addr;
	size_t i;
	volatile char j;

	for(i=0; i<len; i++) {
		*(ptr + i) = 'x';
		j = *(ptr + i);
	}
}

int create_file(char *path, size_t len) {
	int ret = -1;
	int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if(fd == -1) {
		perror("open");
		return -1;
	}

	for(int i=0; i<(len/sizeof(int)); i++) {
		if(write(fd, &i, sizeof(int)) != sizeof(int)) {
			perror("write");
			goto out;
		}
	}

	ret = 0;
out:
	close(fd);
	return ret;
}

int check_file(char *path, size_t len) {
	int ret = -1;
	int fd = open(path, O_RDONLY, 0x0);
	if(fd == -1) {
		perror("open");
		return -1;
	}

	for(int i=0; i<(len/sizeof(int)); i++) {
		int x, r;
		r = read(fd, &x, sizeof(int));
		if(r != sizeof(int)) {
			perror("read");
			goto out;
		}

		if(x != i) {
			fprintf(stderr, "bad file\n");
			goto out;
		}
	}

	ret = 0;
out:
	close(fd);
	return ret;
}

int check_memory(void *addr, size_t len) {
	int i;

	for(i=0; i<(len/sizeof(int)); i++) {
		int x = *(int *)(addr + i*sizeof(int));
		if(x != i) {
			fprintf(stderr, "memory check failed\n");
			return -1;
		}
	}

	printf("memory test for file map successfull!\n");
	return 0;
}

int main(int argc, char *argv[]) {
	/* Anonymous mapping at kernel-decided location */
	void *addr = mmap(NULL, MAP_LEN, PROT_READ | PROT_WRITE, MAP_PRIVATE |
			MAP_ANONYMOUS, 0x0, 0x0);
	if(addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	access_memory(addr, MAP_LEN);
	printf("Anoymous mapping at kernel-decided address: %p\n", addr);
	munmap(addr, MAP_LEN);

	/* Anonmous mapping, request specific location */
	printf("Requesting 0x%x bytes @0x%llx\n", MAP_LEN, REQUESTED_ADDR);
	addr = mmap((void *)REQUESTED_ADDR, MAP_LEN, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0x0, 0x0);
	if(addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	access_memory(addr, MAP_LEN);
	printf("Anoymous mapping at specific address: %p\n", addr);
	munmap(addr, MAP_LEN);

	/* anonymous, fixed */
	addr = mmap((void *)REQUESTED_ADDR, MAP_LEN, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0x0, 0x0);
	if(addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	access_memory(addr, MAP_LEN);
	printf("Anoymous fixed mapping at 0x%llx: %p\n", REQUESTED_ADDR, addr);
	munmap(addr, MAP_LEN);

	/* File mapping */
	if(create_file(FILENAME, FILELEN))
		exit(EXIT_FAILURE);
	if(check_file(FILENAME, FILELEN))
		exit(EXIT_FAILURE);

	int fd = open(FILENAME, O_RDWR, 0x0);
	if(fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	addr = mmap(NULL, MAP_LEN, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0x0);
	printf("File mapping at kernel address: %p\n", addr);

	if(check_memory(addr, MAP_LEN))
		exit(EXIT_FAILURE);

	munmap(addr, MAP_LEN);

	/* File mapping at specific address */
	addr = mmap((void *)REQUESTED_ADDR, MAP_LEN, PROT_READ | PROT_WRITE, MAP_PRIVATE,
			fd, 0x0);
	printf("File mapping at specific address 0x%llx: %p\n", REQUESTED_ADDR,
			addr);

	if(check_memory(addr, MAP_LEN))
		exit(EXIT_FAILURE);

	munmap(addr, MAP_LEN);

	close(fd);

	return 0;
}
