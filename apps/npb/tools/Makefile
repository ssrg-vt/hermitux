CC=gcc
CFLAGS=-g

all: setparams

setparams: setparams.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o setparams npbparams.h
