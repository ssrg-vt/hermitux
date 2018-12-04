HERMIT_INSTALL=/opt/hermit
CC=$(HERMIT_INSTALL)/bin/x86_64-hermit-gcc
LD=$(HERMIT_INSTALL)/bin/x86_64-hermit-ld
LIBS=../../hermitux-kernel/prefix/x86_64-hermit/lib/libhermit.a

CFLAGS= -g -Os
LDFLAGS=-static

all:
	$(CC) $(CFLAGS) -c hermitux-light.c -o hermitux-light.o
	$(LD) hermitux-light.o $(LIBS) -o hermitux-light $(LDFLAGS)
	strip hermitux-light
	gzip < hermitux-light > hermitux-light.gz

clean:
	rm -rf *.o hermitux-light hermitux-light.gz
