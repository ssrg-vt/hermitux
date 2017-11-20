#!/bin/bash

for dir in bt.a bt.b cg.a cg.b ep.a ep.b is.a is.b mg.a mg.b sp.a sp.b ua.a ua.b; do
	cd $dir
	capstan build
	rm -rf $dir.txt
	for i in `seq 1 10`; do
		chrono capstan run | grep -e "Time in" -e "chrono" >> $dir.txt
	done
	cd -
done
