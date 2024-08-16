//The MIT License (MIT)

//Copyright (c) 2024 David Reguera Garcia aka Dreg & Ivan Redondo aka S0S4

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.


// By: @S0S4 & Dreg
// github.com/therealdreg





#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct 
{
    uint32_t io;
    uint32_t par_lng;
    uint32_t cmd_grp;
    uint32_t cmd;
} desar_t;
 
 
void printuint32bin(uint32_t num)
{
    if(!num) return;
 
    printuint32bin(num>>1);
    putchar(((num&1 == 1) ? '1' : '0'));
}
 
int main(int argc, char *argv[])
{
    if(argc < 2){
        perror("./deserialicer.c [IOCTL PACKET]");
        exit(1);
    }

    char *input = argv[1];

    uint32_t ioctl_pck = (uint32_t)strtoul(input, NULL, 16);
    
    desar_t desar = { 0 };
    
    printuint32bin(ioctl_pck);
    printf("b 0x%08X\n", ioctl_pck);
    
    desar.io = ioctl_pck >> 29;
    desar.par_lng = (ioctl_pck & 0x0FFFFFFF) >> 16;
    desar.cmd_grp = (ioctl_pck & 0x0000FFFF) >> 8;
    desar.cmd = ioctl_pck & 0x000000FF;
    
    printuint32bin(desar.io);
    printf("b io: 0x%X %c\n", desar.io, desar.io >= 0x20 && desar.io <= 0x7F ? desar.io : ' ' );
    printuint32bin(desar.par_lng);
    printf("b par_lng: 0x%X %c\n", desar.par_lng, desar.par_lng >= 0x20 && desar.par_lng <= 0x7F ? desar.par_lng : ' ');
    printuint32bin(desar.cmd_grp);
    printf("b cmd_grp: 0x%X %c\n", desar.cmd_grp, desar.cmd_grp >= 0x20 && desar.cmd_grp <= 0x7F ? desar.cmd_grp : ' ');
    printuint32bin(desar.cmd);
    printf("b cmd: 0x%X %c\n", desar.cmd, desar.cmd >= 0x20 && desar.cmd <= 0x7F ? desar.cmd : ' ');
 

    printf("packet: 0x%08X", (desar.io << 29) | (desar.par_lng << 16) | (desar.cmd_grp << 8) | desar.cmd);
 
    return 0;
}
 
