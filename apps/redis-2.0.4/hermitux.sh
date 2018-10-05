#!/bin/bash

VERBOSE=1
INSTALL=/home/pierre/Desktop/hermitux/hermitux-kernel/prefix/
PROXY=$INSTALL/bin/proxy
KERNEL=$INSTALL/x86_64-hermit/extra/tests/hermitux
PROG=./redis-server
#ARGS='--port 8000 --protected-mode no --save "" --appendonly no'
ARGS='redis.conf'
MINIFS=0

HERMIT_MEM=2G HERMIT_VERBOSE=$VERBOSE HERMIT_ISLE=uhyve HERMIT_TUX=1 \
	HERMIT_MINIFS=$MINIFS HERMIT_MINIFS_HOSTLOAD=.minifs HERMIT_NETIF=tap100 \
	$PROXY $KERNEL $PROG $ARGS
