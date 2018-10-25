#!/bin/bash

# Linux:
./prog -N 100 1232896 x

# Hermitux
ARGS="-N 100 1232896 x" make test
