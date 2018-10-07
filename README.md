# Hermitux test environment

## Prerequisites
  - Recommended system: Debian 9 (GlibC support is not assured on newer 
    distributions)
  - `build-essential` debian package, plus [HermitCore prerequisites](https://github.com/RWTH-OS/HermitCore#requirements)
  - [HermitCore toolchain](https://github.com/RWTH-OS/HermitCore#hermitcore-cross-toolchain) installed in /opt/hermit (the one coming from the
  debian repositories mentionned in HermitCore GitHub repositories works fine, you might need to install the `apt-transport-https` debian package before downloading the toolchain packages)
    - You may also need to install a recent version of libmpfr to use the hermit toolchain on debian 9:https://www.mpfr.org/mpfr-current/
  - Libseccomp sources (on debian/ubuntu: libseccomp-dev)
  - PyElfTools to interpret profiling results `sudo pip install pyelftools`
  - For fortran test application, you will need the `gfortran` debian package
  - Clang/LLVM to test this compiler, we recommend the following version to also test the obfuscation options: https://github.com/obfuscator-llvm/obfuscator

TODO here: put prerequisites for syscall rewriting and identification (cmake
with curl support)

## Steps

1. Install everything with the bootstrap script:

```bash
./bootstrap.sh
```

2. Test an example application, for example NPB IS:
```bash
cd apps/loader-npb/loader-npb-is
make test
```

## Template Makefile
TODO describe here

## `hermit-run`
TODO describe here

## Features

- Debugging: TODO describe here
- Profiling: TODO describe here
- secure container: TODO describe here
- Checkpoint/restart: TODO describe here
