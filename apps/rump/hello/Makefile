CFLAGS  = -O3 -g
LDFLAGS=-lm
TARGET = prog
CC=x86_64-rumprun-netbsd-gcc
BAKE=rumprun-bake
RUN=rumprun
MEM?=1G

SRC := $(shell ls *.c)

all: $(TARGET)

$(TARGET): $(TARGET).compiled
	$(BAKE) hw_generic $(TARGET) $(TARGET).compiled

$(TARGET).compiled: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
#	strip $(TARGET).compiled

run: $(TARGET)
	$(RUN) kvm -M $(MEM) -g "-nographic -m $(MEM) -vga none -enable-kvm" -i $(TARGET)

clean:
	rm -rf *.o $(TARGET) $(TARGET).compiled
