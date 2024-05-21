#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>
#include <libusb-1.0/libusb.h>

int main() {
    int fd;

    struct usbdevfs_ctrltransfer request = { 0 };
    request.bRequest = 0x00;
    request.bRequestType = 0x40;


    int rc = 0;


    // Abrir el dispositivo FTDI como un archivo de dispositivo
    fd = open("/dev/bus/usb/001/013", O_RDWR);

    rc = ioctl(fd, USBDEVFS_CONTROL, &request);
    printf("Reset! %d", rc);

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
