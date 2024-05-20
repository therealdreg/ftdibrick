#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>
#include <libusb-1.0/libusb.h>

int main() {
    int fd;
    uint32_t test = 0;

    int rc = 0;


    // Abrir el dispositivo FTDI como un archivo de dispositivo
    fd = open("/dev/bus/usb/001/012", O_RDWR);

    rc = ioctl(fd,USBDEVFS_REAPURBNDELAY, NULL);
    printf("%d", test); 

/*
    // Enviar el comando de reinicio utilizando ioctl
    if (ioctl(fd, USBDEVFS_RESET, 0) < 0) {
        perror("Error al reiniciar el dispositivo FTDI");
        close(fd);
        return 1;
    }
*/

/*

NOTAS:

poll modem -> USBDEVFS_REAPURB 

*/


    // Cerrar el archivo de dispositivo
    close(fd);

    return 0;
}
