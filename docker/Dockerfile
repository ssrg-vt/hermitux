# Base image is debian 9
FROM debian:9

# Install hermitcore toolchain
RUN apt-get update
RUN apt-get install -y apt-transport-https curl cmake bsdmainutils wget vim nano git binutils autoconf automake make cmake qemu-kvm qemu-system-x86 nasm gcc g++ ca-certificates build-essential libtool libgmp-dev libseccomp-dev
RUN echo "deb [trusted=yes] https://dl.bintray.com/hermitcore/ubuntu bionic main" | tee -a /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y --allow-unauthenticated binutils-hermit newlib-hermit pte-hermit gcc-hermit libhermit libomp-hermit

# MPFR
RUN mkdir ~/Software
RUN cd ~/Software && wget https://www.mpfr.org/mpfr-current/mpfr-4.0.2.tar.bz2
RUN cd ~/Software && tar xf mpfr-4.0.2.tar.bz2
RUN cd ~/Software/mpfr-4.0.2 && ./configure && make -j`nproc` && make install && ldconfig

# HermiTux
RUN cd ~ && git clone https://github.com/ssrg-vt/hermitux
RUN cd ~/hermitux && sed -i "s/^HERMITUX_BASE.*/HERMITUX_BASE=\$\(HOME\)\/hermitux/" tools/Makefile.template
RUN cd ~/hermitux && ./bootstrap.sh

CMD cd ~/hermitux && echo "Welcome to the HermiTux container!" && /bin/bash
