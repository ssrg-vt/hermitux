#!/bin/bash

/usr/lib/gcc/x86_64-linux-gnu/6/cc1 -quiet -v -imultiarch x86_64-linux-gnu hello.c -nostdinc -isystem /home/pierre/Desktop/hermitux/musl/prefix/include -isystem /usr/lib/gcc/x86_64-linux-gnu/6/include -quiet -dumpbase hello.c -mtune=generic -march=x86-64 -auxbase hello -g -O1 -version -o /tmp/cckEYJTg.s

as -v --64 -o /tmp/ccKX36Xf.o /tmp/cckEYJTg.s

 /usr/lib/gcc/x86_64-linux-gnu/6/collect2 -plugin /usr/lib/gcc/x86_64-linux-gnu/6/liblto_plugin.so -plugin-opt=/usr/lib/gcc/x86_64-linux-gnu/6/lto-wrapper -plugin-opt=-fresolution=/tmp/ccETAT3e.res -plugin-opt=-pass-through=/usr/lib/gcc/x86_64-linux-gnu/6/libgcc.a -plugin-opt=-pass-through=/usr/lib/gcc/x86_64-linux-gnu/6/libgcc_eh.a -plugin-opt=-pass-through=-lc -plugin-opt=-pass-through=/usr/lib/gcc/x86_64-linux-gnu/6/libgcc.a -plugin-opt=-pass-through=/usr/lib/gcc/x86_64-linux-gnu/6/libgcc_eh.a -dynamic-linker /lib/ld-musl-x86_64.so.1 -nostdlib -shared -o prog /home/pierre/Desktop/hermitux/musl/prefix/lib/Scrt1.o /home/pierre/Desktop/hermitux/musl/prefix/lib/crti.o /usr/lib/gcc/x86_64-linux-gnu/6/crtbeginS.o -L/home/pierre/Desktop/hermitux/musl/prefix/lib -L /usr/lib/gcc/x86_64-linux-gnu/6/. -Bstatic -Bsymbolic /tmp/ccKX36Xf.o -lm /usr/lib/gcc/x86_64-linux-gnu/6/libgcc.a /usr/lib/gcc/x86_64-linux-gnu/6/libgcc_eh.a -lc /usr/lib/gcc/x86_64-linux-gnu/6/libgcc.a /usr/lib/gcc/x86_64-linux-gnu/6/libgcc_eh.a /usr/lib/gcc/x86_64-linux-gnu/6/crtendS.o /home/pierre/Desktop/hermitux/musl/prefix/lib/crtn.o


