#!/usr/bin/env python3


# This tool is for making easier the task of pushing strings to the stack
#  by Dreg

import sys

def string_to_hex_chunks(input_string):
    byte_array = input_string.encode('utf-8')
    hex_blocks = []
    for i in range(0, len(byte_array), 4):
        block = byte_array[i:i+4]
        # Primero ajustar la longitud del bloque antes de revertirlo
        full_block = block + b'\x00' * (4 - len(block))
        reversed_block = full_block[::-1]
        block_int = int.from_bytes(reversed_block, byteorder='big')
        hex_blocks.append(f'push 0x{block_int:08X}')
    for block in reversed(hex_blocks):
        print(block)

def main():
    if len(sys.argv) != 2:
        print("Usage: python script.py 'string'")
        sys.exit(1)

    input_string = sys.argv[1]
    string_to_hex_chunks(input_string)

if __name__ == '__main__':
    main()
