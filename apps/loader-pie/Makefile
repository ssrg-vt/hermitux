HERMITUX_BASE=$(HOME)/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix
MUSL_PREFIX=$(HERMITUX_BASE)/musl/prefix
MUSL_GCC=$(MUSL_PREFIX)/bin/musl-gcc

# For obfuscation, add this to the CFLAGS:
# -mllvm -fla -mllvm -sub -mllvm -bcf
#  https://github.com/obfuscator-llvm/obfuscator
CFLAGS=-g -O2
LDFLAGS=-static -pie

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
# To get a static pie executable it is not easy, you need this toolchain:
# https://github.com/richfelker/musl-cross-make and this fix:
# https://github.com/richfelker/musl-cross-make/issues/47
CC=./musl-cross-make/prefix/bin/x86_64-linux-musl-gcc
#CC=$(MUSL_GCC)
#CC=clang
#CC=gcc

all: $(PROG)

$(CC):
	git clone https://github.com/richfelker/musl-cross-make.git
	cd musl-cross-make; \
		cp config.mak.dist config.mak; \
		echo "TARGET = x86_64-linux-musl" >> config.mak; \
		echo "OUTPUT = $(PWD)/musl-cross-make/prefix" >> config.mak; \
		echo "GCC_CONFIG += --enable-default-pie" >> config.mak; \
		make -j`nproc`; \
		make install

$(PROG): $(CC) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG) $(LDFLAGS)

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

superclean: clean
	rm -rf musl-cross-make

