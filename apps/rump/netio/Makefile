CFLAGS  = -g
LDFLAGS=-lm
TARGET = prog
CC=x86_64-rumprun-netbsd-gcc
BAKE=rumprun-bake
RUN=rumprun
MEM=1G

SRC := server.c

all: $(TARGET) client

client: client.c
	gcc client.c -o client

$(TARGET): $(TARGET).compiled
	$(BAKE) hw_generic $(TARGET) $(TARGET).compiled

$(TARGET).compiled: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(TARGET)
	$(RUN) kvm -M $(MEM) -g "-nographic -m $(MEM) -vga none -enable-kvm" -I if,vioif,'-net tap,script=no,ifname=tap100' -W if,inet,static,10.0.5.3/24 -i $(TARGET)
	

clean:
	rm -rf *.o $(TARGET) $(TARGET).compiled client
