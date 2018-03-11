CFLAGS=-Wall -Werror
LDFLAGS=-lseccomp

all: export-filter

export-filter: export-filter.c
	gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf *.o export-filter
