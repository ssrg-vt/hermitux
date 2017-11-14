HERMIT_LOCAL_INSTALL=../../hermit-compiler/prefix
MUSL_GCC=../../musl/prefix/bin/musl-gcc

CFLAGS=-g
LDFLAGS=-static

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE=0
ISLE=uhyve
KVM=0

all: hello

hello:hello.c
	$(MUSL_GCC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

test: 
	make -B hello
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				hello arg1 arg2 arg3 arg4

gdb:
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=$(ISLE) HERMIT_KVM=$(KVM) \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf *.o hello
