#!/bin/bash

ITERATIONS=10

SIZE=64
MTIT=10000

for i in `seq 1 $ITERATIONS`; do
	./prog $SIZE $MTIT | grep res
done

echo "---"

for i in `seq 1 $ITERATIONS`; do
	make test ARGS="$SIZE $MTIT" | grep res
done
echo "---"

for i in `seq 1 $ITERATIONS`; do
	make test_fast ARGS="$SIZE $MTIT" | grep res
done

