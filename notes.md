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

	printf("Result: %llu\n", stop - start);

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
```
LINUX
Result: 14405692
Result: 14081966
Result: 13955332
Result: 14067852
Result: 14135586
Result: 14036106
Result: 14290898
Result: 14179994
Result: 13942806
Result: 14094688

KVM (catch)
Result: 61172602
Result: 62802182
Result: 167445942
Result: 161251990
Result: 167225052
Result: 156427462
Result: 170259560
Result: 175406026
Result: 97091170
Result: 63631592

uhyve (catch)
Result: 49010934
Result: 48194500
Result: 48555946
Result: 48765228
Result: 47947304
Result: 48476300
Result: 49024454
Result: 49050998
Result: 49986266
Result: 51898732

KVM (call)
Result: 226152
Result: 226164
Result: 226198
Result: 226174
Result: 467146
Result: 225978
Result: 226174
Result: 226158
Result: 225984
Result: 226158

uhyve (call)

Result: 45154
Result: 45274
Result: 46320
Result: 45266
Result: 45202
Result: 45166
Result: 46378
Result: 98784
Result: 45276
Result: 45290
```
