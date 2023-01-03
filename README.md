# HermiTux: a unikernel binary-compatible with Linux applications

**HermiTux is no longer actively maintained, if you are looking for an active unikernel project with similar compatibility goals and methods please make sure to check out [Unikraft](https://unikraft.org/).**

For general information about HermiTux's design principles and implementation, please read the
[VEE'19](https://www.ssrg.ece.vt.edu/papers/vee2019.pdf) and the
[IEEE TC](https://www.ssrg.ece.vt.edu/papers/tc21.pdf) papers. There are also various
[documents](https://github.com/ssrg-vt/hermitux/wiki/Documents) related to HermiTux listed in the
wiki.

The instruction that follows are for x86-64. We have basic support for an ARM64 embedded
board, more information in the [Wiki](https://github.com/ssrg-vt/hermitux/wiki/Aarch64-support).

## Prerequisites
  - Recommended system: Ubuntu 20.04/Debian 11 (GlibC support is not assured
  on other distributions)
    - See [here](https://github.com/ssrg-vt/hermitux/wiki/Old-Linux-distributions-requirements)
    for additional instructions regarding older distributions Ubuntu 18.04/16.04 or Debian 10/9 
  - Debian/Ubuntu packages:
```
sudo apt update
sudo apt install git build-essential cmake nasm apt-transport-https wget \
	libgmp-dev bsdmainutils libseccomp-dev python3 libelf-dev
```

  - HermitCore	toolchain installed in `/opt/hermit`:
```
for dep in binutils-hermit_2.30.51-1_amd64.deb gcc-hermit_6.3.0-1_amd64.deb \
        libhermit_0.2.10_all.deb  newlib-hermit_2.4.0-1_amd64.deb; do \
    wget https://github.com/ssrg-vt/hermitux/releases/download/v1.0/$dep && \
    sudo dpkg -i $dep && \
    rm $dep;
done
```

## Build

1. Clone the repository and retrieve the submodules
```bash
git clone https://github.com/ssrg-vt/hermitux
cd hermitux
git submodule init && git submodule update
```

2. Compile everything as follows:

```bash
make
```

## Run an application

Test an example application, for example NPB IS:
```bash
cd apps/npb/is
# let's compile it as a static binary:
gcc *.c -o is -static
# let's launch it with HermiTux:
sudo HERMIT_ISLE=uhyve HERMIT_TUX=1 ../../../hermitux-kernel/prefix/bin/proxy \
	../../../hermitux-kernel/prefix/x86_64-hermit/extra/tests/hermitux is

# Now let's try with a dynamically linked program:
gcc *.c -o is-dyn
# We can run it by having hermitux execute the dynamic linux loader:
sudo HERMIT_ISLE=uhyve HERMIT_TUX=1 \
	../../../hermitux-kernel/prefix/bin/proxy \
	../../../hermitux-kernel/prefix/x86_64-hermit/extra/tests/hermitux \
	/lib64/ld-linux-x86-64.so.2 ./is-dyn
```

For more documentation about multiple topics, please see the wiki:
[https://github.com/ssrg-vt/hermitux/wiki](https://github.com/ssrg-vt/hermitux/wiki)

HermiTux logo made by [Kerbreizh Informatique](https://www.kerbreizh-informatique.fr/communication/).
