## Welcome to the HermiTux website

HermiTux is a [unikernel](http://unikernel.org/): a minimal operating system
with low memory/disk footprint and sub-second boot time, executing an
application within a single address space on top of an hypervisor. Moreover,
HermiTux is **binary-compatible** with Linux: it can run native Linux
executables.

While being a proof-of-concept, HermiTux supports multiple compiled (C, C++,
Fortran) and interpreted (Python, LUA) languages. It provides binary analysis
and rewriting techniques to optimize system call latency and modularize a
kernel in the presence of unmodified binaries. It supports statically and
dynamically linked programs, different compilers and optimzation levels.
HermiTux also provides basic support for multithreading, debugging and
profiling.

![](https://i.ibb.co/GtMRTDy/graph-unikernel-metrics.png)

HermiTux is based on the [HermitCore](https://hermitcore.org/) operating
system.

### Trying it out
HermiTux is open source and all the code and instructions are on GitHub:
- [https://github.com/ssrg-vt/hermitux](https://github.com/ssrg-vt/hermitux)

### Design Principles

For a detailed description please read HermiTux' [paper]().

HermiTux uses a lightweight KVM-based hypervisor that loads the Linux binary
alongside a minimal OS layer within a single address space virtual machine. At
runtime, system calls made by the application are caught by HermiTux's kernel.

![](https://i.ibb.co/19cLCyw/pic-overview.png)

Optionally, HermiTux provides a mechanism to rewrite system call invocation
into common function calls to the kernel, significantly reducing the system
call latency.

HermiTux can also analyze a Linux binary to determine which system calls it
invokes, and compile a custom kernel containing only the implementations of
these particular system calls.

### People

- [Pierre Olivier](https://sites.google.com/view/pierreolivier), Virginia Tech
- Daniel Chiba, Qualcomm New England
- [Stefan lankes](http://www.acs.eonerc.rwth-aachen.de/cms/E-ON-ERC-ACS/Das-Institut/Mitarbeiter/Lehrstuhl-Leitung/~fqxm/Lankes-Stefan/?lidx=1&allou=1), RWTH Aachen University
- [Changwoo Min](https://sites.google.com/site/multics69/), Virginia Tech
- [Binoy Ravindran](https://ece.vt.edu/people/profile/ravindran), Virginia Tech
