HERMITUX_BASE=/home/pierre/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix
MUSL_PREFIX=$(HERMITUX_BASE)/musl/prefix
MUSL_GCC=$(MUSL_PREFIX)/bin/musl-gcc

# For obfuscation, add this to the CFLAGS:
# -mllvm -fla -mllvm -sub -mllvm -bcf
#  https://github.com/obfuscator-llvm/obfuscator
CFLAGS=-g -O2
LDFLAGS=-static -nostdlib

SRC := server.c
OBJ=$(SRC:.c=.o)
PROG?=server
PROF=0

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE?=0
ISLE=uhyve
KVM=0
STRIP?=1
ARGS?='-s 40960 0 8000'

# Can be $(MUSL_GCC) or clang (and also the obfuscated version of clang)
CC=$(MUSL_GCC)
#CC=../../obfuscator-build/bin/clang

all: $(PROG) client

client: client.c
	gcc client.c -o client

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
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 HERMIT_NETIF=tap100 \
				HERMIT_KVM=$(KVM) HERMIT_PROFILE=$(PROF) \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				$(PROG) $(ARGS)

gdb:
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=qemu HERMIT_KVM=0 \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf *.o $(PROG) client

