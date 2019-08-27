#ifndef BR_ELF_H
#define BR_ELF_H

#include <stdint.h>
int parse_elf(char *f, uint64_t *vaddr, uint64_t *code_size, uint64_t *offset);

#endif /* BR_ELF_H */
