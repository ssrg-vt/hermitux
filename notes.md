# Hermitux notes

## Hermitcore + app compiled from sources with native (linux) musl experiment

1. Musl:
- Need to implement the `__getreent()` function from newlib to be compatible with hermitcore. The current hack I have made in `src/errno/__errno_location.c` (line 8) does not work (This function is related to TLS/errno and I had to comment the errno check on return from syscall in musl, in `src/internal/syscall_ret.c`). Not sure that will be a problem when we actually load the program though.

- **VDSO**: need to disable it somehow, for application calling `gettimeofday` musl tries to access the VDSO and generates a page fault.
  - For now it's fixed, the issue goes away when I call musl initialization functions from the crt file.

2. Syscalls implemented in HermitCore:

- `writev`: just iterate over the io vectors and call write on each one. This is supposed to be atomic so I used a spinlock. 
- `exit_group`: for now this just calls exit **TODO**: that probably won't work in a multi-threaded environment
- `ioctl`: this really depends on the actual command  executed by ioctl! Thus, I plan to implement command by command. For now I only experimented with simple programs so I only have `TIOCGWINSZ`
  - `TIOCGWINSZ`: it's supposed to return the size of the window in characters and pixel. Hermitcore only use serial output so I don't think this matters much, I hardcoded a 24 * 80 window specs to be returned
- `arch_prctl`: musl use it to set the tls address in the `FS` register, for now it is just doing nothing silently, probably need to implement it in the future **TODO**
- `set_tid_address`: musl use it too, doing nothing silently for now, probably need to implement it later **TODO**
- `clock_gettime`: **TODO**, for now just returns `-ENOSYS` (musl is then falling back on gettimeofday which is implemented
- `gettimeofday`: implemented, I used the hermitcore implementation which was originally present in newlib itself. Note that with qemu the time management seems somehow wrong, but not with uhyve.
- `nanosleep`: just translates to a `sys_msleep` for now, I don't take care of any potential interruption by a signal (would need to set the `rem` parameter
- `brk`: implemented. we can now dynamically allocate up to 128k, yay! Sizes superior to that will be a call to mmap ...

## `Syscall` catch impact

### The program tested:
```C
#include /* ... */

#define ITERATIONS	10000

inline static unsigned long long rdtsc(void) {
	unsigned long lo, hi;
	asm volatile ("rdtsc" : "=a"(lo), "=d"(hi) :: "memory");
	return ((unsigned long long) hi << 32ULL | (unsigned long long) lo);
}

int main(int argc, char** argv)
{
	int i;
	unsigned long long start, stop;
	struct winsize sz;

	start = rdtsc();
	for(i=0; i<ITERATIONS; i++) {
		/* the following being replaced by sys_ioctl( ... ) in the non-catch
         * version */
		ioctl(0, TIOCGWINSZ, &sz);
		(void)sz;
	}
	stop = rdtsc();

	printf("%llu\n", stop - start);

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

## Other C libraries

### Glibc
Seems hard to compile with hermit infrastructure, better to wait until the 
loader is ready

### Dietlibc
Compiles fine, hello world working. However, it does not support not having the
VDSO...

## PARSEC

### Blackscholes:
usign mmap ...

## ERRNO
Need to implement it!
