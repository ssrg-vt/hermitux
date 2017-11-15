HERMIT_TOOLCHAIN=/opt/hermit/
HERMIT_LOCAL_INSTALL=../../../hermit-compiler/prefix
MUSL_LIBC=../../../musl-hermitux/lib/libc.a 
MUSL_LOCAL_INSTALL=../../../musl-hermitux/prefix/include

OPTIMIZATION=3

CC=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-gcc
LD=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-ld
CFLAGS=-Wall -Werror -g -isystem $(MUSL_LOCAL_INSTALL) -O$(OPTIMIZATION)\
	   -mcmodel=medium 
LDFLAGS=-static
LIBS= $(MUSL_LIBC) \
	 $(HERMIT_TOOLCHAIN)/x86_64-hermit/lib/libm.a \
	 $(HERMIT_LOCAL_INSTALL)/x86_64-hermit/lib/libhermit.a \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtend.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtbegin.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crti.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtn.o

PROG=ioctl
OBJS=crt0.o ioctl.o

FREQ=3600
VERBOSE=0
ISLE=qemu
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
	HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 HERMIT_ISLE=$(ISLE) \
				   HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

linux:
	gcc -O$(OPTIMIZATION) -nostdinc -isystem ../../musl-hermitux/prefix/include\
		-c ioctl.c -o ioctl-linux.o
	gcc ioctl-linux.o ../../musl-hermitux/prefix/lib/crt1.o \
		-o ioctl.linux -static -nostdlib -L../../musl-hermitux/prefix/lib -lc

clean:
	rm -rf $(PROG) $(PROG).linux *.o
