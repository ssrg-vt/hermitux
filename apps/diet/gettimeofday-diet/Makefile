HERMIT_TOOLCHAIN=/opt/hermit/
HERMIT_LOCAL_INSTALL=../../../hermit-compiler/prefix
MUSL_LIBC=../../../dietlibc-0.33/bin-x86_64/dietlibc.a

CC=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-gcc
LD=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-ld
CFLAGS=-Wall -Werror -g
LDFLAGS=
LIBS= $(MUSL_LIBC) \
	 $(HERMIT_TOOLCHAIN)/x86_64-hermit/lib/libm.a \
	 $(HERMIT_LOCAL_INSTALL)/x86_64-hermit/lib/libhermit.a \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtend.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtbegin.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crti.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtn.o

PROG=gettimeofday

all: $(PROG)

$(PROG): $(PROG).o crt0.o
	$(LD) $^ $(LIBS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test: clean $(PROG)
	HERMIT_ISLE=qemu HERMIT_KVM=0 $(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

gdb: clean $(PROG)
	HERMIT_VERBOSE=1 HERMIT_DEBUG=1 HERMIT_ISLE=qemu HERMIT_KVM=0 \
				   $(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)


clean:
	rm -rf $(PROG) *.o
