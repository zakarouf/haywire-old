#!/bin/sh

CC=clang
AR=ar
OUT="haywire"
LIB="-lzkcollection"
FLAGS="-g -ggdb"
CFILES="$(find src -name '*.c')"

echo "Compiling\n"
echo "Files: $CFILES\n"
$CC -Os -O0 -Wall -Wextra $LIB $FLAGS $CFILES -o $OUT

