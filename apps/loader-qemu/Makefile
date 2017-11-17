HERMITUX_BASE=/home/pierre/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermit-compiler/prefix
MUSL_GCC=$(HERMITUX_BASE)/musl/prefix/bin/musl-gcc

CFLAGS=-g -O3
LDFLAGS=-static

SRC := $(shell ls *.c)
OBJ=$(SRC:.c=.o)
PROG?=prog

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux-qemu
VERBOSE=1
ISLE=qemu
KVM=0
STRIP=0

all: $(PROG)

$(PROG): $(SRC)
	$(MUSL_GCC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 
ifeq ($(STRIP),1)
	strip $(PROG)
endif

objdump:
	objdump --source $(PROG) > /tmp/objdump.txt && vim /tmp/objdump.txt

test: $(PROG)
	make -B $(PROG)
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				$(PROG) $(ARGS)

gdb:
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=qemu HERMIT_KVM=0 \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf *.o $(PROG)

