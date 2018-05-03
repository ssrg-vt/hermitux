# Hermitux test environment

## Prerequisites
  - Recommended system: Debian 9 (GlibC support is not assured on newer 
    distributions)
  - [HermitCore prerequisites](https://github.com/RWTH-OS/HermitCore#requirements) (you might need to install the `apt-transport-https` debian package first)
  - HermitCore toolchain installed in /opt/hermit (the one coming from the
  debian repositories mentionned in HermitCore GitHub repositories works fine)
  - Libseccomp sources (on debian/ubuntu: libseccomp-dev)
  - PyElfTools to interpret profiling results `sudo pip install pyelftools`

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
