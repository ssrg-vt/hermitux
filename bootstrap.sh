#!/bin/bash
set -euo pipefail

HERMIT_REPO=git@github.com:danchiba/hermit-compiler.git
HERMIT_BRANCH=pierre
MUSL_REPO=git@github.com:ssrg-vt/hermitux-musl.git

if [ ! -e /opt/hermit/bin/x86_64-hermit-gcc ]; then
	echo "Hermit toolchain not found..."
	exit 1
fi

# 1. HERMITCORE
git clone $HERMIT_REPO --branch $HERMIT_BRANCH
cd hermit-compiler
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
mkdir -p prefix
./configure --prefix=$PWD/prefix --disable-shared
make -j`nproc` install
cd -

# LLVM with obfuscation plugin (disabled by default)
# git clone -b llvm-4.0 https://github.com/obfuscator-llvm/obfuscator.git
# mkdir obfuscator-build
# cd obfuscation-build
# cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD=X86 -DLLVM_INCLUDE_TESTS=OFF ../obfuscator
# make -j`nproc`
# cd -
