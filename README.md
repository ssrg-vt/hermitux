# Hermitux test environment

For general information about HermiTux's design principles and implementation, please read the [paper](https://www.ssrg.ece.vt.edu/papers/vee2019.pdf) and [Daniel Chiba's MS thesis](https://vtechworks.lib.vt.edu/handle/10919/88865).

## Quick start
The easiest way to try HermiTux is with Docker:
```
docker pull olivierpierre/hermitux
docker run --privileged -it olivierpierre/hermitux
```

## Prerequisites
  - Recommended system: Ubuntu 16.04 or Debian 9 (GlibC support is not assured
  on newer distributions)
  - Debian/Ubuntu packages:
```
sudo apt update
sudo apt install git build-essential cmake nasm apt-transport-https wget \
	libgmp-dev bsdmainutils libseccomp-dev python
```
  - [HermitCore	toolchain](https://github.com/RWTH-OS/HermitCore#hermitcore-cross-toolchain)
	installed in /opt/hermit:

```
echo "deb [trusted=yes] https://dl.bintray.com/hermitcore/ubuntu bionic main" \
	| sudo tee -a /etc/apt/sources.list
sudo apt update
sudo apt install binutils-hermit newlib-hermit pte-hermit gcc-hermit \
	libomp-hermit libhermit
```
  - For Debian 9 abd Ubuntu 16.04, you may also need to install a recent
  version of libmpfr to use the hermit toolchain:

```
wget https://www.mpfr.org/mpfr-current/mpfr-4.0.2.tar.bz2
tar xf mpfr-4.0.2.tar.bz2
cd mpfr-4.0.2
./configure
make -j`nproc`
sudo make install
sudo ldconfig
```

 - Finally for Ubuntu 16.04 you need a recent version of cmake:

```
# It is probably better to uninstall any existing installation:
sudo apt remove cmake
wget https://github.com/Kitware/CMake/releases/download/v3.14.3/cmake-3.14.3.tar.gz
tar xf cmake-3.14.3.tar.gz
cd cmake-3.14.3
./configure
make -j`nproc`
make install
```

## Build

1. Clone the repo
```bash
git clone https://github.com/ssrg-vt/hermitux
```

2. Install everything with the bootstrap script:

```bash
cd hermitux
./bootstrap.sh
```

3. Test an example application, for example NPB IS:
```bash
cd apps/npb/is
# let's compile it as a static binary:
gcc *.c -o is -static
# let's launch it with HermiTux:
HERMIT_ISLE=uhyve HERMIT_TUX=1 ../../../hermitux-kernel/prefix/bin/proxy \
	../../../hermitux-kernel/prefix/x86_64-hermit/extra/tests/hermitux is

# Now let's try with a dynamically linked program:
gcc *.c -o is-dyn
# We can launch it like that (for now it needs a bit more RAM):
HERMIT_ISLE=uhyve HERMIT_TUX=1 HERMIT_MEM=1G \
	../../../hermitux-kernel/prefix/bin/proxy \
	../../../hermitux-kernel/prefix/x86_64-hermit/extra/tests/hermitux \
	../../../musl/prefix/lib/libc.so is-dyn
```

For more documentation about multiple topics, please see the wiki:
[https://github.com/ssrg-vt/hermitux/wiki](https://github.com/ssrg-vt/hermitux/wiki)

HermitCore's Emoji is provided free by [EmojiOne](https://www.emojione.com/).
