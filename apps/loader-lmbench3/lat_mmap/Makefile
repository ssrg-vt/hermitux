HERMITUX_BASE=/home/pierre/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix
MUSL_PREFIX=$(HERMITUX_BASE)/musl/prefix
MUSL_GCC=$(MUSL_PREFIX)/bin/musl-gcc

# For obfuscation, add this to the CFLAGS:
# -mllvm -fla -mllvm -sub -mllvm -bcf
#  https://github.com/obfuscator-llvm/obfuscator
CFLAGS=-g -O3
LDFLAGS=-static -lm

SRC := $(shell ls *.c)
OBJ=$(SRC:.c=.o)
PROG?=prog

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE?=0
ISLE=uhyve
STRIP?=1
# Enable the fine-grained seccomp filter:
SECCOMP?=0
ARGS?=

# Set this to 1 to enable gdb support, see tools/hermitux-gdb. Don't forget
# to disable stripping and -O3 to be able to debug.
DEBUG=0

# Can be $(MUSL_GCC) or clang (and also the obfuscated version of clang)
CC=$(MUSL_GCC)
#CC=clang
#CC=gcc
all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG) $(LDFLAGS)

#$(PROG): $(OBJ)
#	ld -o $(PROG) $(OBJ) $(MUSL_PREFIX)/lib/crt1.o $(MUSL_PREFIX)/lib/libc.a $(LDFLAGS)
#ifeq ($(STRIP),1)
#	strip $(PROG)
#endif

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

objdump:
	objdump --source $(PROG) > /tmp/objdump.txt && vim /tmp/objdump.txt

test: $(PROG)
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
	HERMIT_DEBUG=$(DEBUG) HERMIT_SECCOMP=$(SECCOMP) \
	HERMIT_PROFILE=0 \
	HERMIT_MEM=4G $(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) $(PROG) $(ARGS)

clean:
	rm -rf *.o $(PROG)

