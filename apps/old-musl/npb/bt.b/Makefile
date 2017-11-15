HERMIT_TOOLCHAIN=/opt/hermit/
HERMIT_LOCAL_INSTALL=../../../../hermit-compiler/prefix
MUSL_LIBC=../../../../musl-hermitux/lib/libc.a 
MUSL_LOCAL_INSTALL=../../../../musl-hermitux/prefix/include

CC=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-gcc
LD=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-ld
CFLAGS=-Wall -Werror -g -isystem $(MUSL_LOCAL_INSTALL) -O3 -mcmodel=medium \
	   -Wno-unused-but-set-variable
LDFLAGS=-static
LIBS= $(MUSL_LIBC) \
	 $(HERMIT_TOOLCHAIN)/x86_64-hermit/lib/libm.a \
	 $(HERMIT_LOCAL_INSTALL)/x86_64-hermit/lib/libhermit.a \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtend.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtbegin.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crti.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtn.o

PROG=bt.b
OBJS=bt.o crt0.o c_timers.o c_print_results.o wtime.o add.o adi.o error.o \
	 exact_rhs.o exact_solution.o initialize.o rhs.o set_constants.o \
	 solve_subs.o verify.o x_solve.o y_solve.o z_solve.o print_results.o

FREQ=3600
VERBOSE=0
ISLE=uhyve
KVM=1

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $^ $(LIBS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test: clean $(PROG)
	HERMIT_FREQ=$(FREQ) HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

gdb: clean $(PROG)
	HERMIT_FREQ=$(FREQ) HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=$(ISLE) HERMIT_KVM=$(KVM) \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf $(PROG) *.o
