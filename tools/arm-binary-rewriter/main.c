#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

#define BINARY              "prog"
#define MAP_FILE_OFFSET     0x0
#define MAP_SIZE            0x7000
#define TEXT_VADDR          0x400000
#define TEXT_SECTION_START  0x130
#define TEXT_SECTION_END    (0x130 + 0x5590)
#define SYSCALL_INSTR       0xd4000001
#define HANDLER_ADDR        0x24d150
#define REWRITE_MASK        0x94000000

int main(int argc, char *argv[])
{
    printf("Loading %s, .text vaddr: 0x%llx, .text offset: 0x%llx, "
            ".text size: 0x%llx\n", BINARY, TEXT_VADDR, MAP_FILE_OFFSET,
            MAP_SIZE);

    int fd = open(BINARY, O_RDWR);
    if(!fd) {
        perror("open");
        return -1;
    }

    void *map = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
            MAP_FILE_OFFSET);
    if(map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    uint32_t *ptr = map + TEXT_SECTION_START;
    while(ptr != (map + TEXT_SECTION_END)) {
        uint64_t addr = ((void *)ptr-map)+TEXT_VADDR;
        uint32_t instr = *ptr;
        if(instr == SYSCALL_INSTR) {
            printf("Rewrite 0x%llx: 0x%llx\n", addr, instr);
            printf("  handler @0x%llx == PC (0x%llx) + 0x%llx\n",
                    HANDLER_ADDR, addr, (int32_t)addr-(int32_t)HANDLER_ADDR);
            uint32_t instr_offset = (int32_t)addr - (int32_t)HANDLER_ADDR;
            instr_offset = instr_offset/4;
            instr_offset = ~instr_offset;
            instr_offset += 1;

            instr_offset &= 0x3FFFFFF;


            uint32_t new_instr = REWRITE_MASK | instr_offset;

            printf("new instruction is: 0x%llx\n", new_instr);

            *ptr = new_instr;
            printf("  0x%llx is now: 0x%llx\n", addr, *ptr);

        }
        ptr += 1;
    }

    close(fd);
    return 0;
}

