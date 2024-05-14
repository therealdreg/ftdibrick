#!/usr/bin/env bash

rm -rf poc.o* poc.exe*
nasm.exe -f win32 poc.asm -o poc.o
ld -mi386pe poc.o -o poc.exe