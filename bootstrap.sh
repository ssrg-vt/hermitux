#!/bin/bash
set -euo pipefail

HERMIT_REPO=git@github.com:danchiba/hermit-compiler.git
HERMIT_BRANCH=pierre
MUSL_REPO=git@github.com:ssrg-vt/musl-hermitux.git
MUSL_BRANCH=hermitux

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
git clone $MUSL_REPO --branch $MUSL_BRANCH
cd musl-hermitux
mkdir -p prefix
LD=/opt/hermit/bin/x86_64-hermit-gcc CC=/opt/hermit/bin/x86_64-hermit-gcc CFLAGS=-g ./configure --prefix=prefix --disable-shared
make -j`nproc` install
cd -
