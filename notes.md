# Hermitux notes

## Hermitcore + app compiled from sources with native (linux) musl experiment

1. Musl:
- Need to implement the `__getreent()` function from newlib to be compatible with hermitcore. The current hack I have made in `src/errno/__errno_location.c` (line 8) does not work (This function is related to TLS/errno and I had to comment the errno check on return from syscall in musl, in `src/internal/syscall_ret.c`). Not sure that will be a problem when we actually load the program though.

- **VDSO**: need to disable it somehow, for application calling `gettimeofday` musl tries to access the VDSO and generates a page fault.
  - For now it's fixed, the issue goes away when I call musl initialization functions from the crt file.
  
- `char ** environ` pointer not working the same way as newlib

2. Syscalls implemented in HermitCore:

- `writev`: just iterate over the io vectors and call write on each one. This is supposed to be atomic so I used a spinlock. 
- `exit_group`: for now this just calls exit **TODO**: that probably won't work in a multi-threaded environment
- `ioctl`: this really depends on the actual command  executed by ioctl! Thus, I plan to implement command by command. For now I only experimented with simple programs so I only have `TIOCGWINSZ`
  - `TIOCGWINSZ`: it's supposed to return the size of the window in characters and pixel. Hermitcore only use serial output so I don't think this matters much, I hardcoded a 24 * 80 window specs to be returned
- `arch_prctl`: musl use it to set the tls address in the `FS` register, it is now implemented for the `SET_FS`, `SET_GS`, `GET_FS` and `GET_GS` operations! The way it works is as follows:
  - When an arch_prctl syscall is received, we cannot directly write the updated value in FS: indeed, becasue of our hooking system, a system call is executed followign an interrupt, meaning that on interrupt return, registers saved on the stack when the interrupt was first received will be restored. These include FS value, so we'll end up overwriting the new value we recently set. Thus, what we do is that we update the saved registers on the stack (accessible to a data structure similar to pt_regs in Linux), and let the system restore them, and it will retore the udpated value of FS.
- `set_tid_address`: musl use it too, doing nothing silently for now, probably need to implement it later **TODO**
- `clock_gettime`: done
- `gettimeofday`: implemented, I used the hermitcore implementation which was originally present in newlib itself. Note that with qemu the time management seems somehow wrong, but not with uhyve.
- `nanosleep`: just translates to a `sys_msleep` for now, I don't take care of any potential interruption by a signal (would need to set the `rem` parameter
- `brk`: implemented. we can now dynamically allocate up to 128k, yay! Sizes superior to that will be a call to mmap ...
- `fcntl`: **TODO**
- `unlink`: Implemented! (uhyve & qemu)
- `getpriority`: implemented

## `Syscall` catch impact

### The program tested:
```C
#include /* ... */

extern int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg); 

#define SYSCATCH
#define ITERATIONS	10000

inline static unsigned long long rdtsc(void) { /* ... */ }

int main(int argc, char** argv)
{
	int i;
	unsigned long long start, stop;
	struct winsize sz;
	char buf[128];

	start = rdtsc();
	for(i=0; i<ITERATIONS; i++) {
#ifdef SYSCATCH
		ioctl(0, TIOCGWINSZ, &sz);
#else
		sys_ioctl(0, TIOCGWINSZ, (unsigned long)&sz);
#endif /* SYSCATCH */
		(void)sz;
	}
	stop = rdtsc();

	sprintf(buf, "Result: %llu\n", stop - start);
	write(1, buf, strlen(buf));

	return 0;
}
```

### Setup: everything compiled with `-O3`:
- Native Linux
- Qemu/KVM with syscall catching
- uHyve with syscall catching
- Qemu/KVM with direct call
- uHyve with direct call

### Results
See  in this repo `results/ioctl` (ipython notebook + graph)

## NPB
- BT: working
- CG: working
- DC: not working, using `mmap`
- IS: working
- EP: working
- FT: not working, I'm suspecting an error internal to hermitcore
- LU: not working, page fault
- MG: working
- SP: working
- UA: working

### Performance evaluation

Setup:
- Hermit with syscall catch mechanism, musl, _KVM_
- Hermit with syscall catch, musl, _uHyve_
- Native linux (static compilation with musl)

Everything -O3, class B:
BT, CG, IS, EP, MG, SP, UA

## Other applications
- Postmark: working! **TODO** performance evaluation
- blackscholes: something is wrong with argc/argv

## Other C libraries

### Glibc
Seems hard to compile with hermit infrastructure, better to wait until the 
loader is ready

### Dietlibc
Compiles fine, hello world working. However, it does not support not having the
VDSO...

## PARSEC

## ERRNO
Need to implement it!

## LOADER
1. All references are hardcoded in the application as we only have the binary.
   We don't want to disassemble as it is a complex problem (involving finding
   and reifying all references to moved data/code), and it's not an exact 
   science: some programs just cannto be reassembled. With obfuscation it's 
   even worse.

   So, we assume the application should use the address space as it wants: by
   convention, it is the regular section of the address space given by linux
   to application (doc/x86/mm.txt for the actual values), the lower part of
   the address space.
 
2. It means we have to relocate the hermitcore kernel, currently using the
   entire address space cause it's a single address space kernel, to somewhere
   it will not be overwritten by the loaded application at load and run time.

   We have to choice:
   A. Put it at the same location as linux (highest part of the address space):
   the issue here is that gcc can generate code to address these high 
   addresses only with mmodel=large (medium too?) -> it's ok for C code, but for
   assembly we need to rewrite it by hand and while for most instruction it's 
   easy, it can be complicated for other (i.e. the fact that a 64bit address
   can only come from rax) --> see with Daniel for more detailed description.

   B. Put it below 0x400000:
   It's a convention for non-PIE programs to start at 0x400000 in Linux, this
   address was arbitrarily chosen and there is nothing below. Source:
   https://gist.github.com/CMCDragonkai/10ab53654b2aa6ce55c11cfc5b2432a4
   it is sufficient to put HC there so we put it there
   
   In addition, we have to move the heap which started before after (kernel_start 
   + kernel_size) to (kernel_start + kernel_size + linux_binary_size)

3. Using uhyve, During kernel initialization we also load each loadable section
   from the linux binary. In particular, the .text will start at 0x400000

4. After the kernel is initialized and before we start to the program entry 
   point, we need to setup the stack: indeed, before the main function the c 
   library performs some initialization adn access auxv. See hermitux.c for more
   detais about how to setup the stack. We use inline assembly to push
   argc, argv, envp and auxvs

5. We use inline assembly to jump to the program entry point. Some boilerplate 
   code copy the value of the stack pointer (pointing to the beginning of the
   memory region containing argc, argv, envp, auxv) in rdi (register containing
   the first argument during a function call), then call the C library 
   initialization function. This first (and only) parameter is then used
   to reconstruct argc, argv, envp, and auxv, which are then passed to 
   the application when the C library is done initializaing anf give control
   to the app. GlibC expects some destructor passed by the dynamic loader in
   %rdx at that point, we need to clear it otherwise there is garbage value in
   that register and the CPU ends up jumping to it at application exit time.
   
 ## Compatibility
 
- PARSEC:
  - blackscholes (C)
  - bodytrack (C++)
  - canneal (C++)
  - freqmine (C++)
  - swaptions (C++)
  - canneals (C++)
  - streamcluster (C++)

- NPB (fortran):
  - cg
  - ep
  - ft
