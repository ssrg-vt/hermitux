FROM debian:10

# Install hermitcore toolchain
RUN apt-get update
RUN apt-get install -y apt-transport-https curl cmake bsdmainutils wget vim nano git binutils autoconf automake make cmake qemu-kvm qemu-system-x86 nasm gcc g++ ca-certificates build-essential libtool libgmp-dev libseccomp-dev
RUN echo "deb [trusted=yes] https://dl.bintray.com/hermitcore/ubuntu bionic main" | tee -a /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y --allow-unauthenticated binutils-hermit newlib-hermit pte-hermit gcc-hermit libhermit libomp-hermit

# HermiTux
RUN cd ~ && git clone https://github.com/ssrg-vt/hermitux
RUN cd ~/hermitux && sed -i "s/^HERMITUX_BASE.*/HERMITUX_BASE=\$\(HOME\)\/hermitux/" tools/Makefile.template
RUN cd ~/hermitux && make -j`nproc`

CMD cd ~/hermitux && echo "Welcome to the HermiTux container!" && /bin/bash
