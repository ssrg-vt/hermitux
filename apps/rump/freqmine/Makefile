CFLAGS  = -O3 -g -I. -DENABLE_PARSEC_HOOKS
LDFLAGS=
TARGET = prog
CC=x86_64-rumprun-netbsd-g++
BAKE=rumprun-bake
RUN=rumprun
MEM=10G

SRC := $(shell ls *.cpp)

all: $(TARGET)

$(TARGET): $(TARGET).compiled
	$(BAKE) hw_generic $(TARGET) $(TARGET).compiled

$(TARGET).compiled: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: filesystem $(TARGET)
	$(RUN) kvm -M $(MEM) -g "-nographic -m $(MEM) -vga none -enable-kvm" -i -b fs.iso,/fs $(TARGET)

filesystem:
	genisoimage -l -r -o fs.iso fs

clean:
	rm -rf *.o $(TARGET) $(TARGET).compiled
