#!/bin/bash

VERBOSE=1
INSTALL=/home/pierre/Desktop/hermitux/hermitux-kernel/prefix/
PROXY=$INSTALL/bin/proxy
KERNEL=$INSTALL/x86_64-hermit/extra/tests/hermitux
PROG=./redis-server
#ARGS='--port 8000 --protected-mode no --save "" --appendonly no'
ARGS='redis.conf'
MINIFS=0
PROF=0

HERMIT_MEM=2G HERMIT_VERBOSE=$VERBOSE HERMIT_ISLE=uhyve HERMIT_TUX=1 \
	HERMIT_MINIFS=$MINIFS HERMIT_MINIFS_HOSTLOAD=.minifs HERMIT_NETIF=tap100 \
	HERMIT_PROFILE=$PROF HERMIT_CPUS=1 HERMIT_IP=192.168.1.12 \
	HERMIT_GATEWAY=192.168.1.1 \
	numactl --physcpubind=1 nice -n -10 $PROXY $KERNEL $PROG $ARGS
