#!/usr/bin/env bash

rm main && rm main.o usb.o
nasm -g -F dwarf -f elf main.asm
ld -m elf_i386_fbsd main.o -o main
