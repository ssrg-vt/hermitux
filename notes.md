# Hermitux notes

## Hermitcore + app compiled from sources with native (linux) musl experiment

1. Musl:
- Need to implement the `__getreent()` function from newlib to be compatible with hermitcore. The current hack I have made in `src/errno/__errno_location.c` (line 8) does not work (This function is related to TLS/errno and I had to comment the errno check on return from syscall in musl, in `src/internal/syscall_ret.c`). Not sure that will be a problem when we actually load the program though.

- ""VDSO"": need to disable it somehow, for application calling `gettimeofday` musl tries to access the VDSO and generates a page fault. **TODO**

2. Syscalls implemented in HermitCore:

- `writev`: just iterate over the io vectors and call write on each one. This is supposed to be atomic so I used a spinlock. 
- `exit_group`: for now this just calls exit **TODO**: that probably won't work in a multi-threaded environment
- `ioctl`: this really depends on the actual command  executed by ioctl! Thus, I plan to implement command by command. For now I only experimented with simple programs so I only have `TIOCGWINSZ`
  - `TIOCGWINSZ`: it's supposed to return the size of the window in characters and pixel. Hermitcore only use serial output so I don't think this matters much, I hardcoded a 24 * 80 window specs to be returned
- `arch_prctl`: musl use it to set the tls address in the `FS` register, for now it is just doing nothing silently, probably need to implement it in the future **TODO**
- `set_tid_address`: musl use it too, doing nothing silently for now, probably need to implement it later **TODO**
- `clock_gettime`: **TODO**
