/* Needed to load the program headers */
#define O_RDONLY			0x0
#define SEEK_SET			0x0
typedef unsigned long long size_t;

/* Elf ABI */
#define AT_NULL				0
#define AT_IGNORE			1
#define AT_EXECFD			2
#define AT_PHDR				3
#define AT_PHENT			4
#define AT_PHNUM			5
#define AT_PAGESZ			6
#define AT_BASE				7
#define AT_FLAGS			8
#define AT_ENTRY			9
#define AT_NOTELF			10
#define AT_UID				11
#define AT_EUID				12
#define AT_GID				13
#define AT_EGID				14
#define AT_PLATFORM			15
#define AT_HWCAP			16
#define AT_CLKTCK			17
#define AT_DCACHEBSIZE		19
#define AT_ICACHEBSIZE		20
#define AT_UCACHEBSIZE		21
#define AT_SECURE			23
#define AT_RANDOM			25
#define AT_EXECFN			31
#define AT_SYSINFO_EHDR		33
#define AT_SYSINFO			32

#define EI_NIDENT	(16)

typedef unsigned long uint64_t;
typedef long int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;

typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef	int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef	int64_t  Elf64_Sxword;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Section;
typedef Elf64_Half Elf64_Versym;

typedef struct {
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
  Elf64_Half	e_type;			/* Object file type */
  Elf64_Half	e_machine;		/* Architecture */
  Elf64_Word	e_version;		/* Object file version */
  Elf64_Addr	e_entry;		/* Entry point virtual address */
  Elf64_Off	e_phoff;		/* Program header table file offset */
  Elf64_Off	e_shoff;		/* Section header table file offset */
  Elf64_Word	e_flags;		/* Processor-specific flags */
  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
  Elf64_Half	e_phentsize;		/* Program header table entry size */
  Elf64_Half	e_phnum;		/* Program header table entry count */
  Elf64_Half	e_shentsize;		/* Section header table entry size */
  Elf64_Half	e_shnum;		/* Section header table entry count */
  Elf64_Half	e_shstrndx;		/* Section header string table index */
} Elf64_Ehdr;

typedef struct {
  Elf64_Word	p_type;			/* Segment type */
  Elf64_Word	p_flags;		/* Segment flags */
  Elf64_Off	p_offset;		/* Segment file offset */
  Elf64_Addr	p_vaddr;		/* Segment virtual address */
  Elf64_Addr	p_paddr;		/* Segment physical address */
  Elf64_Xword	p_filesz;		/* Segment size in file */
  Elf64_Xword	p_memsz;		/* Segment size in memory */
  Elf64_Xword	p_align;		/* Segment alignment */
} Elf64_Phdr;

extern const size_t tux_entry;
extern const size_t tux_size;
extern const size_t tux_start_address;
extern const size_t tux_ehdr_phoff;
extern const size_t tux_ehdr_phnum;
extern const size_t tux_ehdr_phentsize;

void inline push_auxv(unsigned long long type, unsigned long long val) {
	asm volatile("pushq %0" : : "r" (val));
	asm volatile("pushq %0" : : "r" (type));
}

#define DIE()	__builtin_trap()

/* Space allocated for the program headers */
char phdr[4096];
Elf64_Ehdr hdr;

char *auxv_platform = "x86_64";
int libc_sd;

extern void __attribute__ ((noreturn)) sys_exit(int status);

int main(int argc, char** argv, char **environ) {
	unsigned long long int libc_argc = argc -1;
	int i, envc;

	/* count the number of environment variables */
	envc = 0;
	for (char **env = environ; *env; ++env) envc++;

	/* We need to push the element on the stack in the inverse order they will
	 * be read by the C library (i.e. argc in the end) */

	/* auxv */
	push_auxv(AT_NULL, 0x0);
	push_auxv(AT_IGNORE, 0x0);
	push_auxv(AT_EXECFD, 0x0);
	push_auxv(AT_PHDR, tux_start_address + tux_ehdr_phoff);
	push_auxv(AT_PHNUM, tux_ehdr_phnum);
	push_auxv(AT_PHENT, tux_ehdr_phentsize);
	push_auxv(AT_RANDOM, tux_start_address); // FIXME get read random bytes
	push_auxv(AT_BASE, 0x0);
	push_auxv(AT_SYSINFO_EHDR, 0x0);
	push_auxv(AT_SYSINFO, 0x0);
	push_auxv(AT_PAGESZ, 4096);
	push_auxv(AT_HWCAP, 0x0);
	push_auxv(AT_CLKTCK, 0x64); // mimic Linux
	push_auxv(AT_FLAGS, 0x0);
	push_auxv(AT_ENTRY, tux_entry);
	push_auxv(AT_UID, 0x0);
	push_auxv(AT_EUID, 0x0);
	push_auxv(AT_GID, 0x0);
	push_auxv(AT_EGID, 0x0);
	push_auxv(AT_SECURE, 0x0);
	push_auxv(AT_SYSINFO, 0x0);
	push_auxv(AT_EXECFN, 0x0);
	push_auxv(AT_DCACHEBSIZE, 0x0);
	push_auxv(AT_ICACHEBSIZE, 0x0);
	push_auxv(AT_UCACHEBSIZE, 0x0);
	push_auxv(AT_NOTELF, 0x0);
	push_auxv(AT_PLATFORM, (uint64_t)auxv_platform);

	/*envp */
	/* Note that this will push NULL to the stack first, which is expected */
	for(i=(envc); i>=0; i--)
		asm volatile("pushq %0" : : "r" (environ[i]));

	/* argv */
	/* Same as envp, pushing NULL first */
	for(i=libc_argc+1;i>0; i--)
		asm volatile("pushq %0" : : "r" (argv[i]));

	/* argc */
	asm volatile("pushq %0" : : "r" (libc_argc));

	/* with GlibC, the dynamic linker sets in rdx the address of some code
	 * to be executed at exit (if != 0), however we are not using it and here
	 * it contains some garbage value, so clear it
	 */
	asm volatile("xor %rdx, %rdx");
	/* finally, jump to entry point */
	asm volatile("jmp *%0" : : "r" (tux_entry));

	return 0;
}

int libc_start(int argc, char **argv, char **environ) {
	return main(argc, argv, environ);
}

void __attribute__ ((noreturn)) exit(int status) {
	sys_exit(status);
}
