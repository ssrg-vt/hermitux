#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

#include <elf.h>

#include "internal.h"

#if (__WORDSIZE == 64)

#define phdr Elf64_Phdr
#define ehdr Elf64_Ehdr
#define shdr Elf64_Shdr
#define sym Elf64_Sym

#else

#define phdr Elf32_Phdr
#define ehdr Elf32_Ehdr
#define shdr Elf32_Shdr
#define sym Elf32_Sym

#endif

const void* vdso_dlsym(const char* elfimage,const char* symbol) {
  /* WARNING: this does NO validation AT ALL. */
  /* It is meant to be used on the VDSO ELF .so mapped into user space
   * by the kernel.  Since the kernel gave it to us, it is trustworthy.
   * A real dlsym would have to check all the pointers and offsets
   * against the file and section sizes, obviously. */
  const ehdr* eh=(ehdr*)elfimage;
  const char* dynstringtable=0;

  {
    size_t i;
    /* first traverse the section list to find ".dynstr"
     * we need the offset of .dynstr because the names in the symbol
     * list are given as offsets inside .dynstr */
    for (i=0; i<eh->e_shnum; ++i) {
      const shdr* sh=(shdr*)(elfimage + eh->e_shoff + i*eh->e_shentsize);
      if (sh->sh_type==3 && (sh->sh_flags&2)) {	// type SHT_STRTAB and flags has SHF_ALLOC
	dynstringtable = elfimage + sh->sh_offset;
	break;
      }
    }
  }

  /* now traverse .dynsym */
  if (dynstringtable) {
    size_t i;
    for (i=0; i<eh->e_shnum; ++i) {
      const shdr* sh=(shdr*)(elfimage + eh->e_shoff + i*eh->e_shentsize);
      if (sh->sh_type==11) {
	size_t j;
//	printf(".dynsym @ %p\n",vdso + sh->sh_offset);
	for (j=0; j*sh->sh_entsize < sh->sh_size; ++j) {
	  const sym* es=(sym*)(elfimage + sh->sh_offset + j*sh->sh_entsize);
//	  printf("%p: %s\n",((shdr*)(elfimage + eh->e_shoff + es->st_shndx * eh->e_shentsize))->sh-offset + es->st_value-eh->e_entry, dynstringtable+es->st_name);
	  if (!strcmp(dynstringtable+es->st_name,symbol)) {
	    const shdr* sec=(shdr*)(elfimage + eh->e_shoff + es->st_shndx*eh->e_shentsize);
	    size_t ofs=es->st_value-sec->sh_addr+sec->sh_offset;
//	    if (ofs>sec->sh_size) return 0;
//	    printf("found symbol \"%s\" at offset %p\n",dynstringtable+es->st_name,es->st_value);
	    return elfimage + ofs;
	  }
	}
      }
      sh = (shdr*)((char*)sh + eh->e_shentsize);
    }
  }
  return 0;
}

