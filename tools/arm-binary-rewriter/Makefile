PROG=arm-binary-rewriter
SRC=main.c

all: $(PROG)

$(PROG): $(SRC)
	gcc $^ -o $@

test: $(PROG)
	./$(PROG)

clean:
	rm -rf *.o $(PROG)
