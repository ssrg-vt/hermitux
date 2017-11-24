#!/bin/bash

rm -rf out
touch out

make run &>> out & \
	sh -c 'tail -n +0 --pid=$$ -f out | { grep -m 1 halted && kill $$ ;}'; echo "kill"

