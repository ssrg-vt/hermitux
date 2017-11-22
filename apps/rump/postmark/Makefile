CFLAGS  = -O3 -g
LDFLAGS=-lm
TARGET = prog
CC=x86_64-rumprun-netbsd-gcc
BAKE=rumprun-bake
RUN=rumprun
MEM=2G

SRC := $(shell ls *.c)

all: $(TARGET)

$(TARGET): $(TARGET).compiled
	$(BAKE) hw_generic $(TARGET) $(TARGET).compiled

$(TARGET).compiled: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(TARGET)
	$(RUN) kvm -M $(MEM) -g "-nographic -vga none -enable-kvm" -i $(TARGET)
	

clean:
	rm -rf *.o $(TARGET) $(TARGET).compiled
