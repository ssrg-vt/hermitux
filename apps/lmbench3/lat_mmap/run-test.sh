#!/bin/bash

# Linux:
./prog -N 10000 10000000 x

# Hermitux
ARGS="-N 10000 10000000 x" make test
