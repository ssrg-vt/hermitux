#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE		4096
#define MAP_LEN			(PAGE_SIZE*10)
#define REQUESTED_ADDR	0x200aed000

void access_memory(void *addr, size_t len) {
	char *ptr = addr;
	size_t i;
	volatile char j;

	for(i=0; i<len; i++) {
		*(ptr + i) = 'x';
		j = *(ptr + i);
	}
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

	return 0;
}
