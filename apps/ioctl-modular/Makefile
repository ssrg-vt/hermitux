HERMIT_TOOLCHAIN=/opt/hermit/
HERMIT_LOCAL_INSTALL=../../hermit-compiler/prefix
MUSL_LIBC=../../musl-hermitux/lib/libc.a 
MUSL_LOCAL_INSTALL=../../musl-hermitux/prefix/include

CC=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-gcc
LD=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-ld
CFLAGS=-Wall -Werror -g -isystem $(MUSL_LOCAL_INSTALL) -O3 -mcmodel=medium \
	   -Wno-unused-but-set-variable -Wno-unused-variable -Wno-format
LDFLAGS=-static #--gc-sections
LIBS= $(MUSL_LIBC) \
	 $(HERMIT_TOOLCHAIN)/x86_64-hermit/lib/libm.a \
	 libhermit.a \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtend.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtbegin.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crti.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtn.o
LINKER_SCRIPT=ls.x

PROG=ioctl
OBJS=$(PROG).o crt0.o
DISABLED_SYSCALLS=mmap brk ioctl nanosleep fcntl unlink gettimeofday \
				  arch_prctl set_tid_address clock_gettime read write \
				  open close lseek readv writev
FREQ=3600
VERBOSE=0
ISLE=qemu
KVM=0

all: kernel $(PROG)

.phony: kernel
kernel:
	./syscall-disabler.py -o libhermit.a -s $(DISABLED_SYSCALLS) \
		-c ../../hermit-compiler

$(PROG): kernel $(OBJS)
	$(LD) -v  $(OBJS) $(LIBS) -o $@ $(LDFLAGS) -T $(LINKER_SCRIPT)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test: clean $(PROG)
	HERMIT_FREQ=$(FREQ) HERMIT_VERBOSE=$(VERBOSE) HERMIT_ISLE=$(ISLE) \
				HERMIT_KVM=$(KVM) $(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG) \

gdb: clean $(PROG)
	HERMIT_FREQ=$(FREQ) HERMIT_VERBOSE=$(VERBOSE) HERMIT_DEBUG=1 \
				HERMIT_ISLE=$(ISLE) HERMIT_KVM=$(KVM) \
				$(HERMIT_LOCAL_INSTALL)/bin/proxy $(PROG)

clean:
	rm -rf $(PROG) *.o test-file.txt isrs.c.obj isrs.c.obj.tmp libhermit.a
