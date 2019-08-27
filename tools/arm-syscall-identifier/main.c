#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <err.h>

#include "br-elf.h"

/* Info about aarch64 instructions encoding:
 * https://static.docs.arm.com/ddi0596/a/DDI_0596_ARM_a64_instruction_set_architecture.pdf
 */

#define SYSCALL_INSTR       0xd4000001
#define MAX_BACKTRACK_LEVEL 10
#define PAGE_SIZE           4096

typedef struct {
    char binary_name[128];  /* input binary path */
    uint64_t code_vaddr; /* code segment start virtual address */
    uint64_t code_size;  /* code segment size */
    uint64_t file_offset; /* start offset in file */
} config;

void print_cfg(const config *cfg);

int main(int argc, char *argv[])
{
    uint64_t HANDLER_ADDR, vaddr, code_size, offset;
    config cfg;

    if(argc != 3)
        errx(-1, "Usage: %s <binary> <wrapper_addr>\n", argv[0]);

    uint64_t wrapper_addr = strtoul(argv[2], NULL, 16);

    /* elf stuff */
    parse_elf(argv[1], &vaddr, &code_size, &offset);
    strcpy(cfg.binary_name, argv[1]);
    cfg.code_vaddr = vaddr;
    cfg.code_size = code_size;
    cfg.file_offset = offset;

    int fd = open(cfg.binary_name, O_RDONLY);
    if(fd == -1)
        errx(EXIT_FAILURE, "cannot open %s\n", cfg.binary_name);

    uint32_t map_size = (cfg.code_size % PAGE_SIZE == 0) ?
        cfg.code_size : ((cfg.code_size / PAGE_SIZE) + 1) * PAGE_SIZE;
    void *map = mmap(NULL, map_size, PROT_READ, MAP_PRIVATE, fd,
            cfg.file_offset);

    if(map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    uint32_t *ptr = map;
    int syscall_num = 0, syscall_identified = 0;
    while(ptr != (map + cfg.code_size)) {
        uint64_t addr = ((void *)ptr-map)+cfg.code_vaddr;
        uint32_t instr = *ptr;

        /* Is it a SVC (syscall instruction)? */
        if(instr == SYSCALL_INSTR) {
            syscall_num++;

            int backtrack_level = 1;
            while(backtrack_level <= MAX_BACKTRACK_LEVEL) {
                uint32_t bt_instr = *(ptr-backtrack_level);
                uint64_t bt_addr = ((void *)(ptr-backtrack_level)-map) +
                    cfg.code_vaddr;

                /* Look for MOV immediate in x8 */
                if((bt_instr >> 20 == 0xd28) && (bt_instr & 0x1F) == 8) {
                    /* extract the syscall identifier which is the immediate */
                    uint16_t id = (bt_instr >> 5) & 0xFFFF;
                    printf("SVC @0x%llx is %u\n", addr, id);
                    syscall_identified++;
                    break;
                }

                backtrack_level++;
            }
                if(backtrack_level > MAX_BACKTRACK_LEVEL)
                    printf("Could not identify syscall @0x%llx\n", addr);

        } else if((instr >> 26) == 0x25) {
            /* This is a BL imm, could be a BL to the wrapper, let's compute
             * the target address */
            uint32_t offset = instr & ~0xFC000000;
            /* sign extension from 26 to 32 bits */
            if (offset & 0x2000000)
                offset |= 0xFC000000;
            int32_t signed_offset = (int32_t)(offset*4);
            uint64_t target_addr = addr + signed_offset;

            if(target_addr == wrapper_addr) {
                syscall_num++;

                int backtrack_level = 1;
                while(backtrack_level <= MAX_BACKTRACK_LEVEL) {
                    uint32_t bt_instr = *(ptr-backtrack_level);
                    uint64_t bt_addr = ((void *)(ptr-backtrack_level)-map) +
                        cfg.code_vaddr;

                    /* Look for MOV immediate in x0 */
                    if((bt_instr >> 20 == 0xd28) && (bt_instr & 0x1F) == 0) {
                        /* extract the syscall identifier which is the immediate */
                        uint16_t id = (bt_instr >> 5) & 0xFFFF;
                        printf("WRAPPER @0x%llx is %u\n", addr, id);
                        syscall_identified++;
                        break;
                    }

                    backtrack_level++;
                }
                    if(backtrack_level > MAX_BACKTRACK_LEVEL)
                        printf("Could not identify syscall @0x%llx\n", addr);
            }


        }

        ptr += 1;
    }

    close(fd);

    printf("Scan done, identified %d/%d syscall invocations (%d\%)\n",
            syscall_identified, syscall_num,
            ((syscall_identified*100)/syscall_num));

    return 0;
}
