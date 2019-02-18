# Edit this with the path to HermiTux base install folder:
HERMITUX_BASE=$(HOME)/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix
MUSL_PREFIX=$(HERMITUX_BASE)/musl/prefix
MUSL_GCC=$(MUSL_PREFIX)/bin/musl-gcc
DLSO=$(MUSL_PREFIX)/lib/libc.so

CFLAGS=-g -O1
CFLAGS_DYN=$(CFLAGS) -fPIC
LDFLAGS=-static -lm
LDFLAGS_DYN=-pie -lm

SRC_SRV := server.c
OBJ_SRV=$(SRC_SRV:.c=.o)
PROG_SRV?=server
SRC_CLI := client.c
OBJ_CLI=$(SRC_CLI:.c=.o)
PROG_CLI?=client

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE?=0
# Should the binary be stripped?
STRIP?=0
# Enable the profiler when != 0, use this number as the number of samples per
# second. Remeber to add -g and remove -O3 for compile flags
PROF?=0
# Command line arguments
ARGS?=-s 40960 0 8000
# Memory to give to the VM
MEM?=2G

# Set to 1 to enable minifs
MINIFS?=0
# File containing the list of files to load from the host when using minifs
HOSTLOAD?=.minifs

# Set this to 1 to enable gdb support, see tools/hermitux-gdb. Don't forget
# to disable stripping and -O3 to be able to debug.
DEBUG?=0

# Network parameters
NETIF=tap100
IP=10.0.5.2
GATEWAY=10.0.5.1

#CC=gcc
#CC=clang
CC=$(MUSL_GCC)

all: $(PROG_SRV) $(PROG_CLI)

$(PROG_SRV): $(SRC_SRV)
	$(CC) $(CFLAGS) $(SRC_SRV) -o $(PROG_SRV) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test: $(PROG_SRV)
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=uhyve HERMIT_TUX=1 \
	HERMIT_DEBUG=$(DEBUG) \
	HERMIT_PROFILE=$(PROF) HERMIT_MINIFS=$(MINIFS) \
	HERMIT_MEM=$(MEM) HERMIT_MINIFS_HOSTLOAD=$(HOSTLOAD) \
	HERMIT_NETIF=$(NETIF) HERMIT_IP=$(IP) HERMIT_GATEWAY=$(GATEWAY) \
	$(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) $(PROG_SRV) $(ARGS)

clean:
	rm -rf *.o $(PROG_SRV) $(PROG_CLI) hermitux-prof.txt checkpoint/

