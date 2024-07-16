#!/bin/sh
rm -rf main
gcc -I/usr/include/libusb-1.0 -lusb-1.0 -o main main.c
