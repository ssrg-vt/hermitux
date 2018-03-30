#!/bin/bash

BENCHS="null open read write"

for b in $BENCHS; do
	echo "$b - LINUX"
	./prog -N 100000 $b
	echo "$b - HERMITUX"
	ARGS="-N 100000 $b" make test
done
