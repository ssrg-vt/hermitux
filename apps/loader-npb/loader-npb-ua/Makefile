HERMIT_LOCAL_INSTALL=../../../hermit-compiler/prefix
MUSL_GCC=../../../musl/prefix/bin/musl-gcc

CFLAGS=-g
LDFLAGS=-static

PROG=ua

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE=1
ISLE=uhyve
KVM=0

all: $(PROG)

$(PROG): adapt.c convect.c c_print_results.c c_timers.c diffuse.c mason.c \
	move.c precond.c print_results.c randdp.c setup.c transfer.c ua.c utils.c \
	verify.c wtime.c 
	$(MUSL_GCC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

objdump:
	objdump --source $(PROG) > objdump.txt

test: $(PROG)
	make -B $(PROG)
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) HERMIT_TUX=1 \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(KERNEL) \
				$(PROG)

gdb:
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=$(ISLE) HERMIT_KVM=$(KVM) \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf *.o $(PROG)




