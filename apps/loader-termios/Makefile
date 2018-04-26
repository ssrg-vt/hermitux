HERMITUX_BASE=/home/pierre/Desktop/hermitux

HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix
MUSL_GCC=$(HERMITUX_BASE)/musl/prefix/bin/musl-gcc
DIET=../../dietlibc/bin-x86_64/dietlibc.a

CFLAGS=-g -O3
LDFLAGS=-static

SRC := $(shell ls *.c)
PROG?=termios

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE=1
ISLE=uhyve
KVM=0
STRIP=1

all: $(PROG)

$(PROG):
	diet gcc $(CFLAGS) $(SRC) -o $(PROG) $(LDFLAGS)
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

