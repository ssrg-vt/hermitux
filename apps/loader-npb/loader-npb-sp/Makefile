HERMIT_LOCAL_INSTALL=../../../hermit-compiler/prefix
MUSL_GCC=../../../musl/prefix/bin/musl-gcc

CFLAGS=-g
LDFLAGS=-static

PROG=sp

KERNEL=$(HERMIT_LOCAL_INSTALL)/x86_64-hermit/extra/tests/hermitux
VERBOSE=1
ISLE=uhyve
KVM=0

all: $(PROG)

$(PROG): add.c adi.c c_print_results.c c_timers.c error.c exact_rhs.c \
	exact_solution.c initialize.c ninvr.c pinvr.c print_results.c randdp.c \
	rhs.c set_constants.c sp.c txinvr.c tzetar.c verify.c wtime.c x_solve.c \
	y_solve.c z_solve.c
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




