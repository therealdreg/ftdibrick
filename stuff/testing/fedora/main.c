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
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/usbdevice_fs.h>

int main(){

  char device_path[32];
  int fd, i;

  printf("Listando dispositivos USB conectados:\n");



    for (i = 0; i < 9; i++) {
      sprintf(device_path, "/dev/bus/usb/001/%03d", i);
      puts(device_path);
      fd = open(device_path, O_RDWR);
      if (fd < 0) {
        //perror("Error al abrir dispositivo");
        continue;
    }
      int rc = ioctl(fd,0x00005514,0);
      if(rc < 0){
        printf("Error restarting device %s \n", device_path);
      }

    }
    return 0;
}
