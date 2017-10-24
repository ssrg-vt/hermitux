CC=gcc
#CFLAGS = -Wall -g
CFLAGS = -Wall -Os -fomit-frame-pointer

ARCH = i386

DYNLINK = diet-linux.so

all: $(DYNLINK)

DLIBS   = ../bin-$(ARCH)/libdl.a ../bin-$(ARCH)/dietlibc.a
LINKOPS = -Wl,-Ttext=0x1080,-N,-eldso_start

$(DYNLINK): ldso_start.S ldso_test.c $(DLIBS)
	$(CROSS)$(CC) -I.. -I../include $(CFLAGS) -c ldso_test.c
	$(CROSS)$(CC) -I.. -I../include $(CFLAGS) -c ldso_start.S
	$(CROSS)$(CC) -nostdlib $(LDFLAGS) -o $@ ldso_start.o ldso_test.o $(DLIBS) -lgcc $(LINKOPS)
	$(CROSS)strip -R .comment -R .note $@

install: $(DYNLINK)
	cp $(DYNLINK) /lib

clean:
	$(RM) *.o *.a *.so *.out *~
