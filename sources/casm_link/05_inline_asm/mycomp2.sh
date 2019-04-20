#!/bin/sh

FILE_BASE=`basename $1 .c`

clang-6.0 -m32 -emit-llvm $1 -c -o "$FILE_BASE.bc"
llc -march=arm "$FILE_BASE.bc" -o "$FILE_BASE.s"
arm-linux-gnueabi-gcc "$FILE_BASE.s"
qemu-arm -L /usr/arm-linux-gnueabi ./a.out
