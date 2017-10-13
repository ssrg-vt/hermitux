HERMIT_TOOLCHAIN=/opt/hermit/
HERMIT_LOCAL_INSTALL=../../hermit-compiler/prefix
MUSL_LIBC=../../musl-hermitux/lib/libc.a 

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

all: prog

prog: prog.o crt0.o
	$(LD) $^ $(LIBS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test: prog
	HERMIT_ISLE=qemu HERMIT_KVM=0 $(HERMIT_LOCAL_INSTALL)/bin/proxy prog

gdb: clean prog
	HERMIT_VERBOSE=1 HERMIT_DEBUG=1 HERMIT_ISLE=qemu HERMIT_KVM=0 \
				   $(HERMIT_LOCAL_INSTALL)/bin/proxy prog


clean:
	rm -rf prog *.o
