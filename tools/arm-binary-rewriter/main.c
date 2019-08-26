#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <gelf.h>
#include <err.h>
#include <string.h>

/* Info about aarch64 instructions encoding:
 * https://static.docs.arm.com/ddi0596/a/DDI_0596_ARM_a64_instruction_set_architecture.pdf
 */

#define SYSCALL_INSTR       0xd4000001
#define REWRITE_MASK_B      0x14000000  /* for branch instructions */
#define REWRITE_MASK_BL     0x94000000  /* for branch and link */
#define RET_CODE            0xd65f03c0
#define PAGE_SIZE           4096

typedef struct {
    char binary_name[128];  /* input binary path */
    uint64_t code_vaddr; /* code segment start virtual address */
    uint64_t code_size;  /* code segment size */
    uint64_t file_offset; /* start offset in file */
} config;

int parse_elf(char *f, config *cfg);
void print_cfg(const config *cfg);

int main(int argc, char *argv[])
{
    uint64_t HANDLER_ADDR;
    config cfg;

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <binary> <handler_addr>\n", argv[0]);
        exit(-1);
    }

    HANDLER_ADDR = (int)strtol(argv[2], NULL, 16);

    /* elf stuff */
    parse_elf(argv[1], &cfg);
    print_cfg(&cfg);

    int fd = open(cfg.binary_name, O_RDWR);
    if(fd == -1)
        errx(EXIT_FAILURE, "cannot open %s\n", cfg.binary_name);

    uint32_t map_size = (cfg.code_size % PAGE_SIZE == 0) ?
        cfg.code_size : ((cfg.code_size / PAGE_SIZE) + 1) * PAGE_SIZE;
    void *map = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
            cfg.file_offset);

    if(map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    uint32_t *ptr = map;
    int syscall_num = 0, syscall_rewritten = 0;
    while(ptr != (map + cfg.code_size)) {
        uint64_t addr = ((void *)ptr-map)+cfg.code_vaddr;
        uint32_t instr = *ptr;

        /* Is it a SVC (syscall instruction)? */
        if(instr == SYSCALL_INSTR) {
            syscall_num++;
            if(*(ptr+1) == RET_CODE) {
                /* There is a ret right after the syscall instruction, we can
                 * add a simple branch without overwriting the return address
                 * in x30, we'll return from the syscall handler directly where
                 * the function calling the syscall was supposed to return */

                /* We are doing a pc-relative jump to the handler. The handler
                 * is in the kernel and will always be inferior to the
                 * application pc */
                uint32_t instr_offset = (int32_t)addr - (int32_t)HANDLER_ADDR;

                /* B/BL instructions takes an immediate offset on 26 bits,
                 * multiplied by 4 to find the actual address */
                instr_offset = instr_offset/4;

                /* We are doing a backward jump so negate the computed offset,
                 * 2's complement and sign extension on the 26 lowest bits */
                instr_offset = ~instr_offset;
                instr_offset += 1;
                instr_offset &= 0x3FFFFFF;


                /* Add the opcode */
                uint32_t new_instr = REWRITE_MASK_B | instr_offset;

                *ptr = new_instr;
                syscall_rewritten++;
            }
        }
        ptr += 1;
    }

    close(fd);

    printf("Rewriting done, got %d/%d syscall invocations (%d\%)\n",
        syscall_rewritten, syscall_num, ((syscall_rewritten*100)/syscall_num));

    return 0;
}

int parse_elf(char *f, config *cfg) {
    int i, fd, exec_segments;
    Elf *e;
    char *id, bytes[5];
    size_t n;
    GElf_Phdr phdr;

    memset(cfg, 0x0, sizeof(config));
    strcpy(cfg->binary_name, f);

    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EXIT_FAILURE, "ELF library initialization "
            "failed: %s", elf_errmsg(-1));

    if ((fd = open(f, O_RDONLY, 0)) < 0)
        err(EXIT_FAILURE, "open \"%s\" failed", f);

    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EXIT_FAILURE, "elf_begin() failed: %s.", elf_errmsg(-1));

    if (elf_kind(e) != ELF_K_ELF)
        errx(EXIT_FAILURE, "\"%s\" is not an ELF object.", f);

    if (elf_getphdrnum(e, &n) != 0)
        errx(EXIT_FAILURE, "elf_getphdrnum() failed: %s.", elf_errmsg(-1));

    exec_segments = 0;
    for (i = 0; i < n; i++) {
        if (gelf_getphdr(e, i, &phdr) != &phdr)
            errx(EXIT_FAILURE, "getphdr() failed: %s.", elf_errmsg(-1));

        /* Look for executable segments */
        if (phdr.p_flags & PF_X) {
            exec_segments++;
            cfg->code_vaddr = phdr.p_vaddr;
            cfg->code_size = phdr.p_filesz;
            cfg->file_offset = phdr.p_offset;
        }
    }

    if(exec_segments != 1)
        errx(EXIT_FAILURE, "More than 1 or no executable segments in %s\n", f);

    (void) elf_end(e);
    (void) close(fd);

    return 0;
}

void print_cfg(const config *cfg) {
    printf("Parsing results for binary %s\n", cfg->binary_name);
    printf("- code_vaddr: 0x%llx\n", cfg->code_vaddr);
    printf("- code_size: 0x%llx\n", cfg->code_size);
    printf("- file_offset: 0x%llx\n", cfg->file_offset);
}
