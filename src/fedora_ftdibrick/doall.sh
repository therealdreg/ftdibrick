#!/usr/bin/env bash

rm main && rm main.o
nasm -g -F dwarf -f elf32 main.asm
ld -m elf_i386 main.o -o ftdibrick_fedora
