#ifndef USB_MIDI_H
#define USB_MIDI_H

#include "usb.h"

#define USB_CLASS_AUDIO		0x01
#define USB_SUBCLASS_MIDI	0x03

#define USB_MIDI_ENDP_DIR_IN	0x80
#define USB_MIDI_ENDP_DIR_OUT	0x00

#define USB_MIDI_DT_HEADER		0x01
#define USB_MIDI_DT_IN_JACK		0x02
#define USB_MIDI_DT_OUT_JACK	0x03
#define USB_MIDI_DT_ELEMENT		0x04

#define USB_MIDI_JACK_TYPE_UNDEFINED	0x00
#define USB_MIDI_JACK_TYPE_EMBEDDED		0x01
#define USB_MIDI_JACK_TYPE_EXTERNAL		0x02

struct midi_jack_in_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bJackType;
	uint8_t bJackID;
	uint8_t iJack;
} __attribute__((packed));
#define USB_MIDI_JACK_IN_SIZE sizeof(struct midi_jack_in_descriptor)

#define MIDI_JACK_IN_COUNT 1
struct midi_header_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdMSC;
	uint16_t wTotalLength;
	struct midi_jack_in_descriptor jack_in[MIDI_JACK_IN_COUNT];
} __attribute__((packed));
#define USB_MIDI_HEADER_SIZE 7

#ifdef USB_DESCRIPTORS
static const struct {
	struct midi_header_descriptor header;
} __attribute__((packed)) midi_functional_descriptors = {
	.header = {
		.bFunctionLength = USB_MIDI_HEADER_SIZE,
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_MIDI_DT_HEADER,
		.bcdMSC = 0x0100,
		.wTotalLength = sizeof(struct midi_header_descriptor),
		.jack_in = {{
			.bLength = USB_MIDI_JACK_IN_SIZE,
			.bDescriptorType = CS_INTERFACE,
			.bDescriptorSubtype = USB_MIDI_DT_IN_JACK,
			.bJackType = USB_MIDI_JACK_TYPE_EMBEDDED,
			.bJackID = 0,
			.iJack = USBSTR_IDX(JACK_IN_0),
		}},
	},
};

static const struct usb_endpoint_descriptor midi_endp[] = {
{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_MIDI_ENDP_DIR_IN | 0x0,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 0,
},
};

static const struct usb_interface_descriptor midi_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 2,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_AUDIO,
	.bInterfaceSubClass = USB_SUBCLASS_MIDI,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = midi_endp,

	.extra = &midi_functional_descriptors,
	.extralen = sizeof(midi_functional_descriptors),
}};

#endif /* USB_DESCRIPTORS */

extern void midi_set_config(usbd_device *usbd_dev);

#endif /* USB_MIDI_H */
