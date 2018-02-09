# Syscall rewriting benchmarks

## Setup

### Microbenchmark 1

Run the ioctl system call within a loop x times, and measure the time taken 
before the start of the loop until after the end.
This benchmark will be run under four scenarios:
1. Regular ioctl system call compiled statically for Linux, and run on Linux.
2. Regular ioctl system call compiled statically for Linux, and run within 
   HermiTux.
3. ioctl system call compiled statically for Linux, rewritten to transform it 
   into a function call, and run within HermiTux.
4. ioctl function call, implemented with the syscall functionality in the same 
   source file, compiled statically and run under HermiTux.

All of the above files will be compiled with O0.

_Pierre's comments:_
- For 4., it's a little bit unclear. I assume this is just the 'syscall as 
  function' to reproduce a regular unikernel environment. I think we should
  change that experience by implementing ioctl in regular hermitcore, and 
  writing an application calling that ioctl. First, this is more representative
  of a real unikernel launch. Second, now that I think about it the super-high
  performance we saw with the previous experiments might be due to the compiler
  being able to optimize a lot because the calling code and the syscall called
  (just a function) are located in the same file (as opposed to the syscall
  being present in a static library as with regular unikernels)

### Microbenchmark 2
Run the ioctl system call within a loop x times. Record the time before each 
system call is invoked, and the time that the system call function is entered, 
then take the difference of these two.
 
This will require modification of the HermiTux as well as Linux kernel code.

This benchmark will be run under two scenarios:
1. Regular ioctl system call compiled statically for Linux, and run on Linux (I 
  haven't implemented this one yet, and it may be difficult).
2. Regular ioctl system call compiled statically for Linux, and run within 
  HermiTux.
3. ioctl system call compiled statically for Linux, rewritten to transform it 
   into a function call, and run within HermiTux.

Measuring the number of cycles taken for a function call will not be meaningful, 
the last time I tried, some of the values were actually negative.
			 
The 'time' for both benchmarks will actually be clock cycles, measured using the 
rdtsc assembly instruction. 

_Pierre's comments:_
- For 1., I think it's pretty easy to implement for Linux. In the early ioctl
  processing code (look [here](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/fs/ioctl.c#n692)), just make a call to rdtsc and printk the result, we'll
  get it with dmesg.
- For the 'function call', let's try with the method I mention for the first
  microbenchmark (regulat Hermitcore with ioctl implementation). It might be a 
  bit slower and if it is the case you'll get some meaningfull results
