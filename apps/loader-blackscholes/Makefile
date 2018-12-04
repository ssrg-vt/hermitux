HERMITUX_BASE=/home/danchiba/Documents/HermiTux/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix
MUSL_PREFIX=$(HERMITUX_BASE)/musl/prefix
MUSL_GCC=$(MUSL_PREFIX)/bin/musl-gcc

# For obfuscation, add this to the CFLAGS:
# -mllvm -fla -mllvm -sub -mllvm -bcf
#  https://github.com/obfuscator-llvm/obfuscator
CFLAGS=-g -O3 -DENABLE_PARSEC_HOOKS -I. -Wno-format
LDFLAGS=-static -nostdlib

SRC := $(shell ls *.c)
OBJ=$(SRC:.c=.o)
PROG?=prog

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE?=0
ISLE=uhyve
KVM=0
STRIP?=0
ARGS?=

# Can be $(MUSL_GCC) or clang (and also the obfuscated version of clang)
CC=$(MUSL_GCC)

all: $(PROG)

$(PROG): $(OBJ)
	ld -o $(PROG) $(OBJ) $(MUSL_PREFIX)/lib/crt1.o $(MUSL_PREFIX)/lib/libc.a $(LDFLAGS)
ifeq ($(STRIP),1)
	strip $(PROG)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

objdump:
	objdump --source $(PROG) > /tmp/objdump.txt && vim /tmp/objdump.txt

test: $(PROG)
	HERMIT_MEM=1G HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				$(PROG) 1 in_64K.txt out

gdb:
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=qemu HERMIT_KVM=0 \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf *.o $(PROG)

