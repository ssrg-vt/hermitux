FROM debian:11

# Install hermitcore toolchain
RUN apt-get update
RUN apt-get install -y apt-transport-https curl cmake bsdmainutils wget vim nano git binutils autoconf automake make cmake qemu-kvm qemu-system-x86 nasm gcc g++ ca-certificates build-essential libtool libgmp-dev libseccomp-dev

RUN for dep in binutils-hermit_2.30.51-1_amd64.deb gcc-hermit_6.3.0-1_amd64.deb \
        libhermit_0.2.10_all.deb  newlib-hermit_2.4.0-1_amd64.deb; do \
    wget https://github.com/ssrg-vt/hermitux/releases/download/v1.0/$dep && \
    dpkg -i $dep && \
    rm $dep; done

# HermiTux
RUN cd ~ && git clone https://github.com/ssrg-vt/hermitux
RUN cd ~/hermitux && sed -i "s/^HERMITUX_BASE.*/HERMITUX_BASE=\$\(HOME\)\/hermitux/" tools/Makefile.template
RUN cd ~/hermitux && make -j`nproc`

CMD cd ~/hermitux && echo "Welcome to the HermiTux container!" && /bin/bash
