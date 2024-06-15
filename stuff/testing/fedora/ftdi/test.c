#include <stdio.h>
#include <stdlib.h>
#include <libftdi1/ftdi.h>
 
int main(void)
{
    int ret;
    struct ftdi_context *ftdi;
    struct ftdi_version_info version;
    unsigned short data = 0;

    if ((ftdi = ftdi_new()) == 0)
   {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
    }

    if ((ret = ftdi_usb_open(ftdi, 0x0403, 0x6001)) < 0)
    {
        ftdi_free(ftdi);
    }
    if ((ret = ftdi_usb_reset(ftdi)) < 0){

        printf("Unable to reset");
        ftdi_free(ftdi);
    }

    if ((ret = ftdi_poll_modem_status(ftdi, &data)) < 0){

        printf("Unable to poll modem");
        ftdi_free(ftdi);
    }
 

    if ((ret = ftdi_set_latency_timer(ftdi, 0x77)) < 0){

        printf("Unable to poll modem");
        ftdi_free(ftdi);
    }



    ftdi_free(ftdi);
 
    return EXIT_SUCCESS;
}