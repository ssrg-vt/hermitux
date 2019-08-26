# Edit this with the path to HermiTux base install folder:
HERMITUX_BASE=$(HOME)/Desktop/hermitux

PROG=arm-binary-rewriter
SRC=main.c
LDFALGS=-lelf
TARGET=prog
HANDLER_ADDR := $(shell nm $(HERMITUX_BASE)/hermitux-kernel/prefix//aarch64-hermit/extra/tests/hermitux | grep br_syscall_handler | cut -f 1 -d " ")

all: $(PROG)

$(PROG): $(SRC)
	gcc $^ -o $@ $(LDFALGS)

test: $(PROG)
	./$(PROG) $(TARGET) 0x$(HANDLER_ADDR)

clean:
	rm -rf *.o $(PROG)
