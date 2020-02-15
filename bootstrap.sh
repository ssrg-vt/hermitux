#!/bin/bash
set -euo pipefail

git submodule init
git submodule update

if [ ! -e /opt/hermit/bin/x86_64-hermit-gcc ]; then
	echo "Hermit toolchain not found, please follow these instructions:"
	echo "https://github.com/hermitcore/libhermit#hermitcore-cross-toolchain"
	exit 1
fi

# 1. HERMITCORE
cd hermitux-kernel
git submodule init
git submodule update

mkdir -p build
mkdir -p prefix

cd build
cmake -DCMAKE_INSTALL_PREFIX=../prefix ..
make -j`nproc` hermit-bootstrap-install
make -j`nproc` install
cd ..
cd ..

# 2. MUSL
cd musl
mkdir -p prefix
./configure --prefix=$PWD/prefix
make -j`nproc` install
cd -

# 3. Libiomp
cd libiomp
mkdir -p build
cd build
cmake -DLIBOMP_ENABLE_SHARED=OFF ..
make -j`nproc`
cd ..
cd ..

# Fixup template Makefile
sed -i "s@HERMITUX_BASE=.*@HERMITUX_BASE=$PWD@" tools/Makefile.template
