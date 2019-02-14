## Welcome to the HermiTux website

HermiTux is a [unikernel](http://unikernel.org/): a minimal operating system
with low memory/disk footprint and sub-second boot time, executing an
application within a single address space on top of an hypervisor. Moreover,
HermiTux is **binary-compatible** with Linux: it can run native Linux
executables.

Although being a proof-of-concept, HermiTux supports multiple compiled (C, C++,
Fortran) and interpreted (Python, LUA) languages. It provides binary analysis
and rewriting techniques to optimize system call latency and modularize a
kernel in the presence of unmodified binaries. It supports statically and
dynamically linked programs, different compilers and optimization levels.
HermiTux also provides basic support for multithreading, debugging and
profiling.

![](https://i.ibb.co/GtMRTDy/graph-unikernel-metrics.png)
*Comparison of disk footprint, boot time, and memory usage for a Linux
VM, a HermiTux/OSv/Rumprun unikernel, and a Docker container. For more
details see our VEE'19 paper.*

HermiTux is based on the [HermitCore](https://hermitcore.org/) operating
system.

### Trying it out
HermiTux is open source and all the code and instructions are on GitHub:
- (link to be provided soon)

### Design Principles

For a detailed description please read Hermitux’s VEE 2019 [paper]() and
[Daniel Chiba’s MS
Thesis](https://github.com/ssrg-vt/hermitux/raw/master/doc/daniel-chiba-ms-thesis.pdf)
at Virginia Tech (links to be available soon).

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

### Contact

[Pierre Olivier](https://sites.google.com/view/pierreolivier), Virginia Tech: polivier *at* vt *dot* edu

* * *

HermiTux is an open-source project of the [Systems Software Research Group](https://www.ssrg.ece.vt.edu/) at [Virginia Tech](https://vt.edu/). 

HermiTux is supported in part by ONR under grants N00014-16-1-2104, N00014-16-1-2711, N00014-16-1-2818. Any opinions, findings, and conclusions or recommendations expressed in this site are those of the author(s) and do not necessarily reflect the views of ONR.

This research and development was supported by the German Federal Ministry of Education and Research under Grant 01IH16010C (Project ENVELOPE).
