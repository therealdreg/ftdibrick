#!/usr/bin/env bash

rm main && rm main.o
nasm -g -F dwarf -f elf main.asm
ld -m elf_i386 main.o -o sosa
