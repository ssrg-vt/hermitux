HERMIT_LOCAL_INSTALL=../../hermitux-kernel/prefix

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE=0
ISLE=uhyve
KVM=0

all: test prog

prog: syscall_asm

syscall_asm: syscall_asm.s
	gcc -c $^ -o $@.o
	ld $@.o -o $@ -static

test: prog
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				syscall_asm

gdb:
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=$(ISLE) HERMIT_KVM=$(KVM) \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf *.o syscall_asm
