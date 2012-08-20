#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libusb.h>

/** a global context pointer for libusb */
libusb_context *lusb_ctx = NULL;

/** a global (for the sake of not passing it around too much) device handle for libusb */
libusb_device_handle *lusb_dev = NULL;

#define USB_VID    0x04d8
#define USB_PID    0x0204
#define USB_EP_OUT 0x01
#define USB_EP_IN  0x81
#define USB_IFACE  0

/**
* A convenience method for sending data to device
*
* \param sendbuf a pointer to a block of memory that should be send
* \param length size of that block of memory
* \param sent if not NULL, the int value pointed to is set to the number of bytes actually transferred
* \return 0 if successful, -1 if we couldn't send the requested number of bytes, otherwise error from libusb
*/
int usb_send(uint8_t *sendbuf, int length, int *sent) {
	int transferred;
	int err = libusb_bulk_transfer(lusb_dev, USB_EP_OUT, sendbuf, length, &transferred, 0);
	if(err) {
		fprintf(stderr, "E: sending data to device: %s\n", libusb_error_name(err));
		return err;
	}
	if(sent != NULL) {
		*sent = transferred;
		return 0;
	}
	if(transferred != length) {
		fprintf(stderr, "E: wrong transfer length, wanted to send %d bytes but sent %d bytes.\n",
			length, transferred);
		return -1;
	}
	return 0;
}

/**
* A convenience method for receiving data from device
*
* \param recbuf pointer to where the data should be stored
* \param length number of bytes to receive
* \param received if not NULL, the int value pointed to is set to the number of bytes actually received
* \return 0 if successful, -1 if the number of received bytes does not match expectations, otherwise error from libusb
*/
int usb_receive(uint8_t *recbuf, int length, int *received) {
	int transferred;
	int err = libusb_bulk_transfer(lusb_dev, USB_EP_IN, recbuf, length, &transferred, 0);
	if(err) {
		fprintf(stderr, "E: reading data from device: %s\n", libusb_error_name(err));
		return err;
	}
	if(received != NULL) {
		*received = transferred;
		return 0;
	}
	if(transferred != length) {
		fprintf(stderr, "E: wrong transfer length, wanted to receive %d bytes but received %d bytes.\n",
			length, transferred);
		return -1;
	}
	return 0;
}

/**
* main routine
*
* \return 0 on success
*/
int main(int argc, char** argv) {
	int err, ret;
	uint8_t buf[64];

	if(argc < 1) exit(-1); /* will never happen */
	if(argc < 2) goto usage;

	memset(buf, 0, 64);

	/* set up USB connection */
	err = libusb_init(&lusb_ctx);
	if(err) {
		fprintf(stderr, "E: initializing libusbx: %s\n", libusb_error_name(err));
		goto fail;
	}

	lusb_dev = libusb_open_device_with_vid_pid(lusb_ctx, USB_VID, USB_PID);
	if(NULL == lusb_dev) {
		fprintf(stderr, "E: cannot find device.");
		goto fail;
	}

	/* claim interface */
	err = libusb_claim_interface(lusb_dev, USB_IFACE);
	if(err) {
		fprintf(stderr, "E: claiming interface: %s\n", libusb_error_name(err));
		goto fail;
	}

#define dumpreg(R,X) \
	fprintf(stderr, "Register dump for <%s> (is 0x%08x = %ddec):\n", R, X, X); \
	fprintf(stderr, " 31:24 | %d %d %d %d  %d %d %d %d | 0x %X %X\n", \
			X & 0X80000000 ? 1 : 0, X & 0X40000000 ? 1 : 0, X & 0X20000000 ? 1 : 0, X & 0X10000000 ? 1 : 0, \
			X & 0X08000000 ? 1 : 0, X & 0X04000000 ? 1 : 0, X & 0X02000000 ? 1 : 0, X & 0X01000000 ? 1 : 0, \
			(X & 0XF0000000) >> 28, (X & 0X0F000000) >> 24); \
	fprintf(stderr, " 23:16 | %d %d %d %d  %d %d %d %d | 0x %X %X\n", \
			X & 0X800000 ? 1 : 0, X & 0X400000 ? 1 : 0, X & 0X200000 ? 1 : 0, X & 0X100000 ? 1 : 0, \
			X & 0X080000 ? 1 : 0, X & 0X040000 ? 1 : 0, X & 0X020000 ? 1 : 0, X & 0X010000 ? 1 : 0, \
			(X & 0XF00000) >> 20, (X & 0X0F0000) >> 16); \
	fprintf(stderr, " 15:08 | %d %d %d %d  %d %d %d %d | 0x %X %X\n", \
			X & 0X8000 ? 1 : 0, X & 0X4000 ? 1 : 0, X & 0X2000 ? 1 : 0, X & 0X1000 ? 1 : 0, \
			X & 0X0800 ? 1 : 0, X & 0X0400 ? 1 : 0, X & 0X0200 ? 1 : 0, X & 0X0100 ? 1 : 0, \
			(X & 0XF000) >> 12, (X & 0X0F00) >> 8); \
	fprintf(stderr, " 07:00 | %d %d %d %d  %d %d %d %d | 0x %X %X\n", \
			X & 0x80 ? 1 : 0, X & 0x40 ? 1 : 0, X & 0x20 ? 1 : 0, X & 0x10 ? 1 : 0, \
			X & 0x08 ? 1 : 0, X & 0x04 ? 1 : 0, X & 0x02 ? 1 : 0, X & 0x01 ? 1 : 0, \
			(X & 0xF0) >> 4, X & 0x0F);

	if(!strcmp("led", argv[1])) {

		fprintf(stderr, "I: toggling LED.\n");

		buf[0] = 0x80;
		usb_send(buf, 64, NULL);

	} else if(!strcmp("getreg", argv[1]) && argc==3) {
		uint32_t reg = strtol(argv[2], NULL, 0);

		fprintf(stderr, "I: read register 0x%08x\n", reg);

		buf[0] = 0x02;
		((uint32_t*)buf)[1] = reg;
		usb_send(buf, 64, NULL);
		usb_receive(buf, 64, NULL);

		reg = ((uint32_t*)buf)[1];
		dumpreg("see above", reg);

	} else if(!strcmp("btn", argv[1])) {

		fprintf(stderr, "I: read info\n");

		buf[0] = 0x81;
		usb_send(buf, 64, NULL);
		usb_receive(buf, 64, NULL);

		fprintf(stderr, "I: btn status (0x%02x): 0x%02x\n", buf[0], buf[1]);

	} else if(!strcmp("devcfg", argv[1])) {

		buf[0] = 0x01;
		usb_send(buf, 64, NULL);
		usb_receive(buf, 64, NULL);

		dumpreg("DEVCFG0", ((uint32_t*)buf)[0]);
		dumpreg("DEVCFG1", ((uint32_t*)buf)[1]);
		dumpreg("DEVCFG2", ((uint32_t*)buf)[2]);
		dumpreg("PORTA", ((uint32_t*)buf)[3]);
		dumpreg("PORTB", ((uint32_t*)buf)[4]);
		dumpreg("PORTC", ((uint32_t*)buf)[5]);

	} else {

		fprintf(stderr, "E: no such command: %s\n", argv[1]);
		goto usage;

	}

	goto quit;

usage:
	fprintf(stderr,
		"USB PIC32MX generic demo\n"
		"\n"
		"SYNTAX:\n"
		"\n"
		"\t%s <COMMAND>\n"
		"\n"
		"<COMMAND> is one of the following:\n"
		"\n"
		"led\n"
		"\n"
		"btn\n"
		"\n"
		"devcfg\n"
		"\n"
		"getreg <address>\n"
		"\n"
		, argv[0]);

	ret = 1;
	goto quit;

fail:
	fprintf(stderr, "E: aborting.\n");
	ret = 1;

quit:
	if(lusb_dev != NULL) {
		libusb_release_interface(lusb_dev, USB_IFACE);
		libusb_close(lusb_dev);
	}
	if(lusb_ctx != NULL) libusb_exit(lusb_ctx);
	exit(ret);
}

