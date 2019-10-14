#!/bin/bash
make clean && \
autoreconf -i && \
# CC="/home/tom/git/hermitux/musl/prefix/bin/musl-gcc -static" \
# CFLAGS="-I/home/tom/git/hermitux/musl/prefix/include-I/home/tom/lighttpd_headers -L/home/tom/git/hermitux/musl/prefix/lib -L/usr/lib/x86_64-linux-gnu/" \
# ./configure && \
CC="musl-gcc " \
LIGHTTPD_STATIC=yes \
CPPFLAGS=-DLIGHTTPD_STATIC \
CFLAGS="-I/home/tom/git/hermitux/musl/prefix/include -L/home/tom/git/hermitux/musl/prefix/lib -L/usr/lib/x86_64-linux-gnu/" \
./configure --prefix=$HOME/musl
make
