#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS	1000000
#define LEN		0x20001000

int main(int argc, char **argv) {
	for(int i=0; i<ITERATIONS; i++) {
		void *ptr = mmap(0, LEN, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if(ptr != (void *)-1) {
//			memset(ptr, 0x0, LEN);
			munmap(ptr, LEN);
		} else {
			printf("ERROR: mmap returned error\n");
			exit(-1);
		}
	}
}
