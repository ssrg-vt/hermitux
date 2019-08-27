# Edit this with the path to HermiTux base install folder:
HERMITUX_BASE=$(HOME)/Desktop/hermitux

PROG=arm-syscall-identifier
SRC=main.c br-elf.c
LDFLAGS=-lelf
TARGET=prog

WRAPPER_ADDR=$(shell nm $(TARGET) | grep -e __syscall_cp | grep -v __syscall_cp_c | cut -d " " -f 1)

all: $(PROG)

$(PROG): $(SRC)
	gcc $^ -o $@ $(LDFLAGS)

test: $(PROG)
	echo $(WRAPPER_ADDR)
	./$(PROG) $(TARGET) $(WRAPPER_ADDR)

clean:
	rm -rf *.o $(PROG)
