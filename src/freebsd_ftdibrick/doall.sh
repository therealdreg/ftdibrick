#!/usr/bin/env bash

rm main && rm main.o usb.o
nasm -g -F dwarf -f elf main.asm
ld -m elf_i386 main.o -o main
strip main
brandelf -t FreeBSD main
