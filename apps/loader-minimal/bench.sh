#!/bin/bash

HERMITUX_BASE=/home/pierre/Desktop/hermitux
HERMIT_LOCAL_INSTALL=$HERMITUX_BASE/hermitux-kernel/prefix
KERNEL=$HERMIT_LOCAL_INSTALL/x86_64-hermit/extra/tests/hermitux

ITERATIONS=10
PROG=prog

#1. Linux
#for i in `seq 1 $ITERATIONS`; do
#chrono ./$PROG
#done

#2. Hermit
for i in `seq 1 $ITERATIONS`; do
HERMIT_MEM=9M HERMIT_ISLE=uhyve HERMIT_TUX=1 	chrono $HERMIT_LOCAL_INSTALL/bin/proxy $KERNEL \
				./$PROG
done

