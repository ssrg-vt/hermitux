
HERMITUX_BASE=$(HOME)/Desktop/hermitux
HERMIT_LOCAL_INSTALL=$(HERMITUX_BASE)/hermitux-kernel/prefix

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE=0

all: prog

prog: syscall_asm

syscall_asm: syscall_asm.s
	gcc -c $^ -o $@.o
	ld $@.o -o $@

test: prog
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=uhyve HERMIT_TUX=1 \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				syscall_asm

clean:
	rm -rf *.o syscall_asm
