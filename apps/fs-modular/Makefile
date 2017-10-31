HERMIT_TOOLCHAIN=/opt/hermit/
HERMIT_LOCAL_INSTALL=../../hermit-compiler/prefix
MUSL_LIBC=../../musl-hermitux/lib/libc.a 
MUSL_LOCAL_INSTALL=../../musl-hermitux/prefix/include

CC=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-gcc
LD=$(HERMIT_TOOLCHAIN)/bin/x86_64-hermit-ld
CFLAGS=-Wall -Werror -g -isystem $(MUSL_LOCAL_INSTALL) -O3 -mcmodel=medium \
	   -Wno-unused-but-set-variable -Wno-unused-variable -Wno-format
LDFLAGS=-static 
LIBS= $(MUSL_LIBC) \
	 $(HERMIT_TOOLCHAIN)/x86_64-hermit/lib/libm.a \
	 $(HERMIT_LOCAL_INSTALL)/x86_64-hermit/lib/libhermit.a \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtend.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtbegin.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crti.o \
	 $(HERMIT_TOOLCHAIN)/lib/gcc/x86_64-hermit/6.3.0/crtn.o

PROG=fs
OBJS=$(PROG).o crt0.o
DISABLED_SYSCALLS=-DDISABLE_SYS_MMAP -DDISABLE_SYS_BRK -DDISABLE_SYS_IOCTL \
				  -DDISABLE_SYS_NANOSLEEP -DDISABLE_SYS_GETPID \
				  -DDISABLE_SYS_FCNTL -DDISABLE_SYS_UNLINK \
				  -DDISABLE_SYS_GETTIMEOFDAY -DDISABLE_SYS_ARCH_PRCTL \
				  -DDISABLE_SYS_SET_TID_ADDRESS \
				  -DDISABLE_SYS_SYS_CLOCK_GETTIME -DDISABLE_SYS_EXIT \
				  -DDISABLE_SYS_READ

FREQ=3600
VERBOSE=0
ISLE=qemu
KVM=0

all: $(PROG)

.phony: kernel

kernel:
	cd $(HERMIT_LOCAL_INSTALL); cmake -DCMAKE_INSTALL_PREFIX=../prefix -DHERMITUX_DISABLED_SYSCALLS="$(DISABLED_SYSCALLS)" ..
	touch $(HERMIT_LOCAL_INSTALL)/../arch/x86/kernel/isrs.c
#	make -C $(HERMIT_LOCAL_INSTALL)/../build install

$(PROG): $(OBJS)
	$(LD) $(OBJS) $(LIBS) -o $@ $(LDFLAGS)

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
	rm -rf $(PROG) *.o test-file.txt
