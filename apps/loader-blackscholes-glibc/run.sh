#!/bin/bash

INPUT=in_10M.txt
PROG=prog

# 1. Linux
#for i in `seq 1 10`; do
#	./$PROG 1 $INPUT out | grep ROI | cut -d ":" -f 2 | cut -d " " -f 2
#done

#echo "---"

# 2. Hermitux
for i in `seq 1 10`; do
	make test | grep ROI | cut -d ":" -f 2 | cut -d " " -f 2
done
