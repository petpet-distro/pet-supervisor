#!/bin/sh

set -e; # exit on errir

mkdir -p build/bin

cc -O2 -o build/bin/pet-supervisor src/index.c
