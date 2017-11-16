#!/bin/sh

BENCH=ua
CLASS=B
ITERATIONS=10
OUTPUT=$BENCH.$CLASS.txt
SETPARAMS=../tools/setparams

#0. Clean and generate parameters
rm -rf $OUTPUT
rm -rf npbparams.h
$SETPARAMS $BENCH $CLASS

#1. Hermitux
for i in `seq 1 $ITERATIONS`; do
	make PROG=$BENCH test >> $OUTPUT
done

echo "------- HERMITUX ABOVE, LINUX BELOW ---------------------" >> $OUTPUT

#2. Linux
for i in `seq 1 $ITERATIONS` ; do
	chrono ./$BENCH | grep -e "Time in" -e "chrono" >> $OUTPUT
done
