#!/bin/bash

INPUT=in_10M.txt
PROG=prog
ITERATIONS=1

# 1. Linux
echo "Linux:"
for i in `seq 1 $ITERATIONS`; do
	./$PROG 1 $INPUT out | grep ROI | cut -d ":" -f 2 | cut -d " " -f 2
done

echo "---"

# 2. Hermitux
echo "Htux:"
for i in `seq 1 $ITERATIONS`; do
	make test | grep ROI | cut -d ":" -f 2 | cut -d " " -f 2
done
