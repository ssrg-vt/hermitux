# Copyright (C) 2014 Cloudius Systems, Ltd.
#
# This work is open source software, licensed under the terms of the
# BSD license as described in the LICENSE file in the top-level directory.

CXXFLAGS  = -O3 -g -Wall -fPIC -DHAVE_MREMAP=0 -DSQLITE_THREADSAFE=0

TARGET = hello

OBJ_FILES = $(SRC:.c=.o)

SRC := $(shell ls *.c)

quiet = $(if $V, $1, @echo " $2"; $1)
very-quiet = $(if $V, $1, @$1)

all: $(TARGET).so

%.o: %.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(TARGET).so: $(OBJ_FILES)
	$(CC) $(CXXFLAGS) -shared -o $(TARGET).so $^

clean:
	$(call quiet, rm -f $(TARGET).so $(OBJ_FILES), CLEAN)
