CFLAGS  = -O3 -g -DSQLITE_THREADSAFE=0
LDFLAGS=-lm
TARGET = prog
CC=/home/pierre/Desktop/rumprun/prefix/bin/x86_64-rumprun-netbsd-gcc
BAKE=/home/pierre/Desktop/rumprun/prefix/bin/rumprun-bake
RUN=/home/pierre/Desktop/rumprun/prefix/bin/rumprun
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
