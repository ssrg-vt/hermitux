# Note that for usign hermitux_light, the kernel cmake must first be called
# with -DNO_NET=1

HERMITUX_BASE=/home/pierre/Desktop/hermitux
HERMIT_TOOLCAHIN_INSTALL=/opt/hermit

CC=$(HERMIT_TOOLCAHIN_INSTALL)/bin/x86_64-hermit-gcc
LD=$(HERMIT_TOOLCAHIN_INSTALL)/bin/x86_64-hermit-ld
LIBS=$(HERMITUX_BASE)/hermitux-kernel/prefix/x86_64-hermit/lib/libhermit.a

CFLAGS= -Os
LDFLAGS=-static

all:
	$(CC) $(CFLAGS) -c hermitux-light.c -o hermitux-light.o
	$(LD) hermitux-light.o $(LIBS) -o hermitux-light $(LDFLAGS)
	strip hermitux-light
	gzip < hermitux-light > hermitux-light.gz

clean:
	rm -rf *.o hermitux-light hermitux-light.gz
