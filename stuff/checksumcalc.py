#!/usr/bin/env python3

#The MIT License (MIT)

#Copyright (c) 2024 David Reguera Garcia aka Dreg & Ivan Redondo aka S0S4

#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.


# By: @S0S4 & Dreg
# github.com/therealdreg

#checksum calculator EEPRROM FT232R algorithm

import sys

def calculate_checksum(file_path):
    checksum = 0xAAAA
    with open(file_path, "rb") as file:
        data = file.read()

    required_length = 0x3f*2
    if len(data) < required_length:
        print(f"Error: File too short. Needs to be at least {required_length} bytes.")
        return sys.exit(1)
        
    for i in range(0, 0x3f*2, 2):
        if i+1 < len(data):
            word = data[i] | (data[i+1] << 8)
            tmpsum = word ^ checksum
            checksum = tmpsum << 1
            if tmpsum & 0x8000:
                checksum |= 0x01
            checksum &= 0xFFFF
            
    hex_little_endian = f"checksum (little endian hex): 0x{checksum:X}"
    big_endian = ((checksum & 0xFF) << 8) | ((checksum >> 8) & 0xFF)
    hex_big_endian = f"checksum (big endian hex): 0x{big_endian:X}"
    high_byte = (checksum >> 8) & 0xFF
    low_byte = checksum & 0xFF
    high_byte_be = (big_endian >> 8) & 0xFF  
    low_byte_be = big_endian & 0xFF     
    bytes_separated_be = f"checksum (big endian bytes separated): 0x{high_byte_be:02X} 0x{low_byte_be:02X}"
    bytes_separated = f"checksum (little endian bytes separated): 0x{high_byte:02X} 0x{low_byte:02X}"
    
    checksum_index = 0x3f*2
    if checksum_index + 1 < len(data):
        stored_checksum = data[checksum_index] | (data[checksum_index+1] << 8)
    else:
        stored_checksum = 0
    
    return stored_checksum, checksum, hex_little_endian, bytes_separated, hex_big_endian, bytes_separated_be

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <path_to_file>")
        sys.exit(1)

    file_path = sys.argv[1]
    stored_checksum, checksum, hex_little_endian, bytes_separated, hex_big_endian, bytes_separated_be = calculate_checksum(file_path)
    print(f"stored checksum: 0x{stored_checksum:02X}")
    print(f"calculated checksum: 0x{checksum:02X}")
    if stored_checksum == checksum:
        print("     OK!! MATCH!!")
    else:
        print("WARNING!! DOES NOT MATCH!!")
    print(hex_little_endian)
    print(bytes_separated)
    print(hex_big_endian)
    print(bytes_separated_be)
