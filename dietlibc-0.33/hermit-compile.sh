#!/bin/bash

make clean
make CC=/opt/hermit/bin/x86_64-hermit-gcc LD=/opt/hermit/bin/x86_64-hermit-ld CFLAGS=-g -j8
