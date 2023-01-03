<p align="center">
  <img width="200" src="https://github.com/ssrg-vt/hermitux/raw/master/doc/logo.png">
<link rel="shortcut icon" type="image/png" href="/doc/favicon.png">
</p>

* * *
[![](https://img.shields.io/github/stars/ssrg-vt/hermitux?style=social)](https://github.com/ssrg-vt/hermitux)
[![](https://img.shields.io/badge/paper-VEE'19-orange)](https://www.ssrg.ece.vt.edu/papers/vee2019.pdf)
[![](https://img.shields.io/badge/paper-IEEE%20TC'21-orange)](https://www.ssrg.ece.vt.edu/papers/tc21.pdf)

**News**
- Check out this [Usenix ;login: article](https://www.usenix.org/publications/loginonline/unikraft-and-coming-age-unikernels) regarding POSIX/Linux compatibility for unikernels
- An [extended version of HermiTux' paper](https://www.ssrg.ece.vt.edu/papers/tc21.pdf) was accepted in IEEE Transactions on Computers
- We have ported HermiTux to the ARM aarch64 architecture, more info on the [wiki](https://github.com/ssrg-vt/hermitux/wiki/Aarch64-support)
- Our HermiTux [paper](https://www.ssrg.ece.vt.edu/papers/vee2019.pdf) won the best
paper award at [VEE 2019](https://dl.acm.org/citation.cfm?id=3313817).

* * *

HermiTux is a [unikernel](http://unikernel.org/): a minimal operating system
with low memory/disk footprint and sub-second boot time, executing an
application within a single address space on top of a hypervisor. Moreover,
HermiTux is **binary-compatible** with Linux: it can run native Linux
executables.

Although being a proof-of-concept, HermiTux supports multiple compiled (C, C++,
Fortran) and interpreted (Python, LUA) languages. It provides binary analysis
and rewriting techniques to optimize system call latency and modularize a
kernel in the presence of unmodified binaries. It supports statically and
dynamically linked programs, different compilers and optimization levels.
HermiTux also provides basic support for multithreading, debugging and
profiling.

HermiTux is based on the [HermitCore](https://hermitcore.org/) operating
system.

### Trying it out
HermiTux is open source and all the code and instructions are on
[GitHub](https://github.com/ssrg-vt/hermitux).

### Design Principles

For a detailed description please read Hermitux’s VEE 2019
[paper](https://www.ssrg.ece.vt.edu/papers/vee2019.pdf) as well as the 2021
IEEE TC
[paper](https://www.ssrg.ece.vt.edu/papers/tc21.pdf).

HermiTux uses a lightweight KVM-based hypervisor that loads the Linux binary
alongside a minimal OS layer within a single address space virtual machine. At
runtime, system calls made by the application are caught by HermiTux's kernel.

<p align="center">
  <img width="400" src="https://github.com/ssrg-vt/hermitux/raw/master/doc/pic-overview.png">
</p>

Optionally, HermiTux provides a mechanism to rewrite system call invocation
into common function calls to the kernel, significantly reducing the system
call latency.

HermiTux can also analyze a Linux binary to determine which system calls it
invokes, and compile a custom kernel containing only the implementations of
these particular system calls.

There are also various
[documents](https://github.com/ssrg-vt/hermitux/wiki/Documents) related to
HermiTux listed in the wiki.

### Contact

[Pierre Olivier](https://sites.google.com/view/pierreolivier), The University of Manchester: pierre.olivier *at* manchester.ac.uk

* * *

HermiTux is an open-source project of the [Systems Software Research Group](https://www.ssrg.ece.vt.edu/) at [Virginia Tech](https://vt.edu/). 

HermiTux is supported in part by ONR under grants N00014-16-1-2104, N00014-16-1-2711, and N00014-16-1-2818. Any opinions, findings, and conclusions or recommendations expressed in this site are those of the author(s) and do not necessarily reflect the views of ONR. This research and development is also supported by the German Federal Ministry of Education and Research under Grant 01IH16010C (Project ENVELOPE), and by the UK's Engineering and Physical Sciences Research Council under grant EP/V012134/1.

HermiTux logo made by [Kerbreizh Informatique](https://www.kerbreizh-informatique.fr/communication/).
