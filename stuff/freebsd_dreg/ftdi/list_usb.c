/*
The MIT License (MIT)

Copyright (c) 2024 David Reguera Garcia aka Dreg & Ivan Redondo aka S0S4

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


 By: @S0S4 & Dreg
 github.com/therealdreg
*/

#include <dev/usb/usb.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>
#if __FreeBSD_version >= 800069
#include <dev/usb/usb_ioctl.h>
#endif

#define USB_MAX_DEVNAMES 16
#define USB_MAX_DEVNAMELEN 32

int main() {
  char device_path[32];
  int fd, i, j;
  struct usb_device_info device_info;

  puts("by dreg");

  printf("Listando dispositivos USB conectados:\n");

  for (j = 0; j < 10; j++) {
    for (i = 0; i < 10; i++) {
      sprintf(device_path, "/dev/ugen%d.%d", j, i);
      // puts(device_path);
      fd = open(device_path, O_RDWR);
      if (fd < 0) {
        // perror("Error al abrir dispositivo");
        continue;
      }

      // Intentar obtener información del dispositivo
      if (ioctl(fd, USB_GET_DEVICEINFO, &device_info) < 0) {
        perror("ioctl falló");
      } else {

        printf("\n%s\n", device_path);
        printf("  Bus: %u\n", device_info.udi_bus);
        printf("  Address: %u\n", device_info.udi_addr);
        printf("  Producto: %s\n", device_info.udi_product);
        printf("  Vendedor: %s\n", device_info.udi_vendor);
        printf("  Versión: %s\n", device_info.udi_release);
        printf("  Número de Producto: %u 0x%X\n", device_info.udi_productNo,
               device_info.udi_productNo);
        printf("  Número de Vendedor: %u 0x%X\n", device_info.udi_vendorNo,
               device_info.udi_vendorNo);
        printf("  Número de Versión: %u 0x%X\n", device_info.udi_releaseNo,
               device_info.udi_releaseNo);
        printf("  Clase: %u\n", device_info.udi_class);
        printf("  Subclase: %u\n", device_info.udi_subclass);
        printf("  Protocolo: %u\n", device_info.udi_protocol);
        printf("  Configuración Número: %u\n", device_info.udi_config_no);
        printf("  Velocidad: %u\n", device_info.udi_speed);
        printf("  Número de Puertos: %d\n", device_info.udi_nports);
        printf("  Consumo de Energía: %d mA\n", device_info.udi_power);
        printf("  Número de Serie: %s\n", device_info.udi_serial);
      }

      close(fd);
    }
  }

  return 0;
}
