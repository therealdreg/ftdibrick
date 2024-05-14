#!/usr/bin/env bash

rm -rf helloworld hellworld.o && nasm -f elf helloworld.asm && ld -m elf_i386_fbsd -o helloworld -s helloworld.o
