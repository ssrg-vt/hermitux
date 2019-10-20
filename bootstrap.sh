#!/bin/bash
set -euo pipefail

HERMIT_REPO=https://github.com/ssrg-vt/hermitux-kernel.git
MUSL_REPO=https://github.com/ssrg-vt/hermitux-musl.git
MUSL_BRANCH=hermitux-v1.1.18
LIBIOMP_REPO=https://github.com/llvm-mirror/openmp.git
LIBIOMP_BRANCH=release_40

if [ ! -e /opt/hermit/bin/x86_64-hermit-gcc ]; then
	echo "Hermit toolchain not found, please follow these instructions:"
	echo "https://github.com/hermitcore/libhermit#hermitcore-cross-toolchain"
	exit 1
fi

if [ -e hermitux-kernel ] || [ -e libiomp ] || [ -e musl ]; then
	read -p "Delete hermitux-kernel/ libiomp/ musl/ ? " -n 1 -r
	echo
	if [[ $REPLY =~ ^[Yy]$ ]]; then
		    rm -rf hermitux-kernel/ libiomp/ musl/
	else
		echo "Exiting."
		exit
	fi
fi

# 1. HERMITCORE
git clone $HERMIT_REPO
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
git clone $MUSL_REPO musl
cd musl
git checkout $MUSL_BRANCH
mkdir -p prefix
./configure --prefix=$PWD/prefix
make -j`nproc` install
cd -

# 3. Libiomp
git clone $LIBIOMP_REPO libiomp
cd libiomp
git checkout $LIBIOMP_BRANCH
mkdir -p build
cd build
cmake -DLIBOMP_ENABLE_SHARED=OFF ..
make -j`nproc`
cd ..
cd ..
