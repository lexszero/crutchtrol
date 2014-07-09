#ifndef USB_H
#define USB_H

#include "common.h"
#include <libopencm3/usb/usbd.h>

#define USB_STRING_TABLE \
	X(MANUFACTURER,	"LexsZero") \
	X(PRODUCT,		"Crutchtrol") \
	X(SERIAL,		"0xDEADBEEF") \
	X(JACK_IN_0,	"Jack 0") \
	X(ELEMENT_0,	"Infrared element") \

#define X(name, value) USB_STRING_##name,
enum {
USB_STRING_TABLE
};
#undef X

#define USBSTR_IDX(name) (USB_STRING_##name + 1)

extern usbd_device *usb;

extern void task_usb(void *arg);

#endif /* USB_H */
