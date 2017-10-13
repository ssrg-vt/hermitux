CC=/opt/hermit/bin/x86_64-hermit-gcc
LD=/opt/hermit/bin/x86_64-hermit-ld
CFLAGS=-Wall -Werror -g
LDFLAGS=
LIBS= musl/lib/libc.a \
	 /opt/hermit/x86_64-hermit/lib/libm.a \
	 /opt/hermit/x86_64-hermit/lib/libhermit.a \
	 /opt/hermit/lib/gcc/x86_64-hermit/6.3.0/crtend.o \
	 /opt/hermit/lib/gcc/x86_64-hermit/6.3.0/crtbegin.o \
	 /opt/hermit/lib/gcc/x86_64-hermit/6.3.0/crti.o \
	 /opt/hermit/lib/gcc/x86_64-hermit/6.3.0/crtn.o

all: prog

prog: prog.o crt0.o
	$(LD) $^ $(LIBS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test: prog
	HERMIT_ISLE=qemu HERMIT_KVM=0 /opt/hermit/bin/proxy prog

gdb: clean prog
	HERMIT_VERBOSE=1 HERMIT_DEBUG=1 HERMIT_ISLE=qemu HERMIT_KVM=0 /opt/hermit/bin/proxy prog


clean:
	rm -rf prog *.o
