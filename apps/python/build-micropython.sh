#!/bin/bash

echo "Installing pkg-config and libffi if needed ..."
sudo apt install libffi-dev pkg-config

echo "Installing micropython"
git clone --depth=1 https://github.com/micropython/micropython.git
cd micropython

git submodule update --init
make -C ports/unix axtls -j`nproc`
LDFLAGS_EXTRA=-static MICROPY_PY_THREAD=0 make -C ports/unix -j`nproc`

cd -

ln -sf $PWD/micropython/ports/unix/micropython ./mp
