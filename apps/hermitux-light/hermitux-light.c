#define tux_start_address	0x400000

extern const unsigned long long tux_entry;
extern const unsigned long long tux_size;

extern void __attribute__ ((noreturn)) sys_exit(int status);

int main(int argc, char** argv, char **environ)
{
	unsigned long long int libc_argc = argc -1;
	int i, envc;

	envc = 0;
	for (char **env = environ; *env; ++env) envc++;
	
	for(i=0; i<38*2; i++)
		asm volatile("pushq %0" : : "i" (0x00));

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

	asm volatile("jmp *%0" : : "r" (tux_entry));

	return 0;
}

int libc_start(int argc, char **argv, char **environ) {
	return main(argc, argv, environ);
}

void __attribute__ ((noreturn)) exit(int status) {
	sys_exit(status);
}
