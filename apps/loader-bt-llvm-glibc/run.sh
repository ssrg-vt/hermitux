#!/bin/bash

ITERATIONS=10

echo "Linux:"
for i in `seq 1 $ITERATIONS`; do
	./prog | grep "Time in" | sed -rn "s/^ Time in seconds = [	 ]+([0-9]+\.[0-9]+)/\1/p"
done

echo "Hermitux:"
for i in `seq 1 $ITERATIONS`; do
	make test | grep "Time in" | sed -rn "s/^ Time in seconds = [	 ]+([0-9]+\.[0-9]+)/\1/p"
done
