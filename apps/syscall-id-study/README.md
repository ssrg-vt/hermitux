# Syscall identification tests

Usage:

0. Prerequisites: you might have to install gfortran (it is not in 
build-essential):

```
sudo apt install gfortran
```

1. Go to `common/` and edit the first variable of all Makefile templates to
point to the correct install path for hermitux

3. We have a set of dfferent applications that should yield different syscall 
usage:
  - `null`: a C application which `main()` returns directly
  - `hello-world`: C hello world application
  - `hello-world-glibc`: C hello world application with glibc (all the other C 
     apps here are compiled with newlib)
  - `malloc-test`: the C malloc-test benchmark exercizing dynamic memory 
     allocation (`brk`/`sbrk` and `mmap`)
  - `blackscholes`: PARSEC blackscholes (in C)
  - `hourglass`: another C system benchmark
  - `canneal-c++` PARSEC cannel (in c++)
  - `npb-bt-fortran`: fortran inplemenration of NPB BT
  - `sqlite`: a simple C program accessing a sqlite database

4. To compile, simply go to a benchmark directory and use make. You'll notice
   the `-function-sections` and `-fdata-sections` compiler flags are used in
   conjunction with the `--gc-sections` linker flag. This saves a few `syscall`
   call site in the binary (not that much unfortunately)

5. I currently have no access to the syscall identifier tool so I'm just looking
  at how many syscalls instructions are present in each binary. It can be check
  by typing, in each benchmark directoy:

```
make count_syscall_invocations
```

This yields quite different numbers for each benchmarks, and I have good hope 
that the syscall identifier will also give different results.
