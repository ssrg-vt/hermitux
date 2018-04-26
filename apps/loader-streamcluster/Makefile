HERMITUX_BASE=/home/pierre/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix

# For obfuscation, add this to the CFLAGS:
# -mllvm -fla -mllvm -sub -mllvm -bcf
#  https://github.com/obfuscator-llvm/obfuscator
CFLAGS=-g -O3 -DENABLE_PARSEC_HOOKS
LDFLAGS=-lm -static -lpthread

SRC := $(shell ls *.cpp)
SRC_C := $(shell ls *.c)
PROG?=prog

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE?=0
ISLE=uhyve
KVM=0
STRIP?=1
ARGS?=

# Can be $(MUSL_GCC) or clang (and also the obfuscated version of clang)
CC=g++

all: $(PROG)

$(PROG): $(SRC) $(SRC_C)
	$(CC) $(CFLAGS) $(SRC) $(SRC_C) -o $(PROG) $(LDFLAGS)
ifeq ($(STRIP),1)
	strip $(PROG)
endif

objdump:
	objdump --source $(PROG) > /tmp/objdump.txt && vim /tmp/objdump.txt

test: $(PROG)
	HERMIT_MEM=5G HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				$(PROG) $(ARGS)

gdb:
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=qemu HERMIT_KVM=0 \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf *.o $(PROG)

