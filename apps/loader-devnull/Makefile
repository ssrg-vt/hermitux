HERMITUX_BASE=$(HOME)/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix
MUSL_PREFIX=$(HERMITUX_BASE)/musl/prefix
MUSL_GCC=$(MUSL_PREFIX)/bin/musl-gcc

# For obfuscation, add this to the CFLAGS:
# -mllvm -fla -mllvm -sub -mllvm -bcf
#  https://github.com/obfuscator-llvm/obfuscator
CFLAGS=-g -O1
LDFLAGS=-static -lm

SRC := $(shell ls *.c)
OBJ=$(SRC:.c=.o)
PROG?=prog

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE?=0
ISLE=uhyve
# Should the binary be stripped?
STRIP?=0
# Enable the fine-grained seccomp filter:
SECCOMP?=0
# Enable the profiler when != 0, use this number as the number of samples per
# second. Remeber to add -g and remove -O3 for compile flags
PROF?=0
ARGS?=
MINIFS?=0
MEM?=2G
HOSTLOAD?=.minifs

# Set this to 1 to enable gdb support, see tools/hermitux-gdb. Don't forget
# to disable stripping and -O3 to be able to debug.
DEBUG?=0

# Can be $(MUSL_GCC) or clang (and also the obfuscated version of clang)
CC=$(MUSL_GCC)
#CC=clang
#CC=gcc

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG) $(LDFLAGS)
ifeq ($(STRIP),1)
	strip $(PROG)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

objdump:
	objdump --source $(PROG) > /tmp/objdump.txt && vim /tmp/objdump.txt

objdump-kernel:
	objdump --source $(KERNEL) > /tmp/objdump.txt && vim /tmp/objdump.txt

test: $(PROG)
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
	HERMIT_DEBUG=$(DEBUG) HERMIT_SECCOMP=$(SECCOMP) \
	HERMIT_PROFILE=$(PROF) HERMIT_MINIFS=$(MINIFS) \
	HERMIT_MEM=$(MEM) HERMIT_MINIFS_HOSTLOAD=$(HOSTLOAD) \
	$(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) $(PROG) $(ARGS)

clean:
	rm -rf *.o $(PROG) hermitux-prof.txt checkpoint/

