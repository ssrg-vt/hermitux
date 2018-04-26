# Copyright (C) 2014 Cloudius Systems, Ltd.
#
# This work is open source software, licensed under the terms of the
# BSD license as described in the LICENSE file in the top-level directory.

CXXFLAGS  = -O3 -g -Wall -fPIC $(INCLUDES) -fopenmp
LDFLAGS   = -L/home/pierre/Desktop/hermitux/libiomp/build/runtime/src -lomp -lm

TARGET = hello

OBJ_FILES = $(SRC:.c=.o)

SRC := $(shell ls *.c)

quiet = $(if $V, $1, @echo " $2"; $1)
very-quiet = $(if $V, $1, @$1)

all: $(TARGET).so

%.o: %.c
	$(call quiet, $(CC) $(CXXFLAGS) -c -o $@ $<, CXX $@)

$(TARGET).so: $(OBJ_FILES)
	$(call quiet, $(CC) $(CXXFLAGS) -shared -o $(TARGET).so $^ $(LDFLAGS), LINK $@)

clean:
	$(call quiet, rm -f $(TARGET).so $(OBJ_FILES), CLEAN)
