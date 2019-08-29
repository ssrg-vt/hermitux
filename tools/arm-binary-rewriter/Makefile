# Edit this with the path to HermiTux base install folder:
HERMITUX_BASE=$(HOME)/Desktop/hermitux

PROG=arm-binary-rewriter
SRC=main.c br-elf.c
LDFLAGS=-lelf
TARGET=prog
HANDLER_ADDR := $(shell nm $(HERMITUX_BASE)/hermitux-kernel/prefix//aarch64-hermit/extra/tests/hermitux | grep br_syscall_handler | cut -f 1 -d " ")

all: $(PROG)

$(PROG): $(SRC)
	gcc $^ -o $@ $(LDFLAGS)

test: $(PROG)
	./$(PROG) $(TARGET) 0x$(HANDLER_ADDR) syscall-list.txt

clean:
	rm -rf *.o $(PROG)
