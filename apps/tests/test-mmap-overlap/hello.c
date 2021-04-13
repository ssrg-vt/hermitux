#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define LEN		0x20002000

int main(int argc, char **argv) {
    void *ptr1 = mmap(0, LEN, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if(ptr1 == (void *)-1) {
        printf("ERROR: mmap ptr1 returned error\n");
        exit(-1);
    }

    printf("Allocated 0x%llx bytes @%p\n", LEN, ptr1);

    memset(ptr1, '@', LEN);
    for(int i=0; i<LEN; i++) {
        char *array = (char *)ptr1;
        if(array[i] != '@') {
            printf("Memory verification failed!\n");
            return -1;
        }
    }


    void *ptr2 = ptr1 + LEN/2;
    while((size_t)ptr2%0x1000) ptr2 += 1;

    printf("Now trying to allocate 0x%llx bytes @%p\n", LEN, ptr2);
    ptr2 = mmap(ptr2, LEN, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if(ptr2 == (void *)-1) {
        printf("ERROR: mmap ptr2 returned error\n");
        perror("mmap");
        exit(-1);
    }

    printf("trying memset ptr1\n");
    memset(ptr1, '1', LEN/2);
    for(int i=0; i<LEN/2; i++) {
        char *array = (char *)ptr1;
        if(array[i] != '1') {
            printf("Memory verification failed!\n");
            return -1;
        }
    }

    printf("trying memset ptr2\n");
    memset(ptr2, '2', LEN);
    for(int i=0; i<LEN; i++) {
        char *array = (char *)ptr2;
        if(array[i] != '2') {
            printf("Memory verification failed!\n");
            return -1;
        }
    }

    printf("all checks successfull\n");
}
