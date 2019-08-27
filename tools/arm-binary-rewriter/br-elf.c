#include "br-elf.h"

#include <gelf.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/* Return information about the code segment from a binary:
 * - vaddr: virtual address where the code segment is loaded
 * - code_size: size of the code segment
 * - offset: offset in the file from which the code segment is mapped
 */
int parse_elf(char *f, uint64_t *vaddr, uint64_t *code_size, uint64_t *offset) {
    int i, fd, exec_segments;
    Elf *e;
    char *id, bytes[5];
    size_t n;
    GElf_Phdr phdr;

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
            *vaddr = phdr.p_vaddr;
            *code_size = phdr.p_filesz;
            *offset = phdr.p_offset;
        }
    }

    if(exec_segments != 1)
        errx(EXIT_FAILURE, "More than 1 or no executable segments in %s\n", f);

    (void) elf_end(e);
    (void) close(fd);

    return 0;
}


