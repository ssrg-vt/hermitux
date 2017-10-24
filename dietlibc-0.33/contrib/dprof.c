/*
   Copyright (C) 2002 Thomas M. Ogrisegg

   This is free software. You can redistribute and
   modify it under the terms of the GNU General Public
   Public License.

   esize - List size of symbols of an object file
   dprof - display call graph and profile data

   Syntax:
     esize [objectfile] (if objectfile is omitted, a.out will be used)
     dprof [monitorfile] [objectfile] (dito)

   The output of dprof is similiar to the SystemV/R3-prof program.

   btw.:
      ls -l /usr/bin/gprof
      -rwxr-xr-x   1 root     root       112172 Jul 30  2000 /usr/bin/gprof
      ls -l dprof
      -rwxr-xr-x   1 tom      users        5348 Apr  6 19:09 dprof

   While dprof is useful to determine where your program spends
   most of its time, esize is useful to find out where your program
   "spends" most of it's size.
*/

#include <sys/gmon.h>
#include <sys/gmon_out.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <elf.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define GMON_OUT "gmon.out"
#define A_OUT    "test"

#define xmalloc(x) (x *) calloc (1, sizeof (x))

#if defined(__alpha__) || defined(__sparc64__)
# define ElfW(type) Elf64_##type
# define ELFX_ST_TYPE ELF64_ST_TYPE
# define ELFX_ST_BIND ELF64_ST_BIND
#else
# define ElfW(type) Elf32_##type
# define ELFX_ST_TYPE ELF32_ST_TYPE
# define ELFX_ST_BIND ELF32_ST_BIND
#endif

typedef struct __symbol__ {
	char          *name;
	unsigned long length;
	unsigned long addr;
	unsigned long call_cnt;
	unsigned long prof_cnt;
	struct __symbol__ *next;
	struct __symbol__ *prev;
} symbol_t;

enum { STDIN, STDOUT, STDERR };

static symbol_t *sym_base = NULL;
static unsigned long lowpc, highpc;

static void
die (const char *str)
{
	write (STDOUT, str, strlen (str));
	exit (1);
}

static symbol_t *
lookup_symbol (unsigned long addr)
{
	symbol_t *sym = sym_base;

	if (addr > highpc || addr < lowpc)
		return (NULL);
	while (sym) {
		if (sym->addr <= addr && sym->addr+sym->length >= addr)
			return (sym);
		sym = sym->next;
	}
	return (NULL);
}

static void
add_sym (ElfW(Sym) *esym, char *str)
{
	symbol_t *sb = sym_base;
	symbol_t *sym = xmalloc (symbol_t);
	sym->name = str;
	sym->length = esym->st_size;
	sym->addr   = esym->st_value;
	if (!sb) {
		sym_base = sym;
		return;
	}
	if (!sym->length) {
		sym->next = sym_base;
		sym_base->prev = sym;
		sym_base = sym;
		return;
	}
	while (sb) {
		if (!sb->next || (sym->length >= sb->length && 
				sym->length <= sb->next->length)) {
			sym->next = sb->next;
			sb->next  = sym;
			sym->prev = sb;
			break;
		}
		sb = sb->next;
	}
	while (sym_base->prev) sym_base = sym_base->prev;
}

static void
build_symbol_list (void *base)
{
	ElfW(Ehdr) *ehdr = (ElfW(Ehdr) *) base;
	ElfW(Shdr) *shdr = (ElfW(Shdr) *) (base + ehdr->e_shoff), 
	*bshdr = shdr;
	unsigned long l;
	for (l=0;l<ehdr->e_shnum;l++,shdr++) {
	 if (shdr->sh_type == SHT_SYMTAB) {
		ElfW(Sym) *esym = (ElfW(Sym) *) (base + shdr->sh_offset);
		char *str;
		unsigned long ul = shdr->sh_size / sizeof (*esym), uf;
		for (uf=0;uf<ul;uf++, esym++) {
			char type = ELFX_ST_TYPE(esym->st_info);
			if (!esym->st_name) continue;
			if (!(( type & STT_FUNC)  || (type & STT_OBJECT))) continue;
			if (ELFX_ST_BIND (esym->st_info) & STB_WEAK) continue;
			str = (char *) (base+bshdr[shdr->sh_link].sh_offset+esym->st_name);
			add_sym (esym, str);
		}
	 }
	}
}

int
main (int argc, char *argv[])
{
	int fdg, fdp;
	unsigned long len;
	void *gp, *obj;
	static char *object = A_OUT, *monf = GMON_OUT;
	char *self = argv[0] + strlen (argv[0]) - 4;
	long esize = (*(long *) self == *(long *) "size");

	if (argc != 1) 
		if (esize) object = argv[1];
		else {
			monf = argv[1];
			if (argv[2]) object = argv[2];
		}

	if (!esize && ((fdg = open (monf, O_RDONLY)) < 0))
		die (GMON_OUT" could not be opened\n");

	if ((fdp = open (object, O_RDONLY)) < 0)
		die (A_OUT" could not be opened\n");

	if (!esize) {
	 if ((gp = (char *) mmap (NULL, len = lseek (fdg, 0, SEEK_END),
			PROT_READ | PROT_WRITE, MAP_PRIVATE, fdg, 0)) == MAP_FAILED)
		die ("Error mmaping monitor-file\n");

	 if (*(long *) gp != *(long *) "gmon")
		die ("No valid monitorfile specified\n");
	}

	if ((obj = mmap (NULL, (unsigned long ) lseek (fdp, 0, SEEK_END), 
			PROT_READ | PROT_WRITE, MAP_PRIVATE, fdp, 0)) == MAP_FAILED)
		die ("Error mmaping object file\n");
	else if (*(long *) obj != *(long *) ELFMAG)
		die ("Invalid ELF header\n");

	build_symbol_list (obj);

	if (esize) {
		symbol_t *sym = sym_base;
		while (sym) {
			if (sym->length)
				printf ("%-45s: %ld\n", sym->name, sym->length);
			sym = sym->next;
		}
		return (0);
	}
	gp  += sizeof (struct gmon_hdr);
	len -= sizeof (struct gmon_hdr);

	while (len--) {
	  switch (*(char *) gp++) {
		case GMON_TAG_TIME_HIST:
		{
		 struct gmon_hist_hdr *ghdr = (struct gmon_hist_hdr *) gp;
		 lowpc  = *(long *) ghdr->low_pc;
		 highpc = *(long *) ghdr->high_pc;
		 gp  += sizeof (struct gmon_hist_hdr);
		 len -= sizeof (struct gmon_hist_hdr);
		 {
			long l = (*(long *) ghdr->hist_size) << 1;
			char *buf = gp;
			symbol_t *sym;
			gp  += l;
			len -= l;
			while (l--)
				if (buf[l] && ((sym = lookup_symbol (lowpc+(l<<1))))) 
					sym->prof_cnt += (short) buf[l];
		 }
		 break;
		}
		case GMON_TAG_CG_ARC:
		{
		 struct rawarc *arc = (struct rawarc *) gp;
		 symbol_t *sym1 = lookup_symbol (arc->raw_frompc);
		 symbol_t *sym2 = lookup_symbol (arc->raw_selfpc);
		 gp  += sizeof (*arc);
		 len -= sizeof (*arc);
		 sym2->call_cnt += arc->raw_count;
		 if (sym1 && sym2)
			printf ("%s called %s %ld times\n", sym1->name, sym2->name,
				arc->raw_count);
		}
	  }
	}
	{
		symbol_t *sym = sym_base;
		long count = 0;
		const double hundred = 100.0;
		double dcount, ff;

		do if (sym->prof_cnt) count += sym->prof_cnt; 
		while ((sym = sym->next));

		dcount = (double) count;
		ff = hundred/count;

		printf ("\nName           %%Time    Seconds   #Calls\n");
		sym = sym_base;
		while (sym) {
			if (sym->prof_cnt || sym->call_cnt)
			printf ("%-15s %-4.02f    %.02ld.%-8.02ld %2ld\n", sym->name, 
					(double) ff * sym->prof_cnt,
					sym->prof_cnt/100, sym->prof_cnt%100,
					sym->call_cnt);
			sym = sym->next;
		}
	}
	return (0);
}
