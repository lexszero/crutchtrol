#ifndef USB_MIDI_H
#define USB_MIDI_H

#include "usb.h"

#define USB_CLASS_AUDIO		0x01
#define USB_SUBCLASS_MIDI	0x03

#define ENDP_DIR_IN		0x80
#define ENDP_DIR_OUT	0x00

#define DT_HEADER		0x01
#define DT_IN_JACK		0x02
#define DT_OUT_JACK	0x03
#define DT_ELEMENT		0x04

#define DT_ENDP_MS_GENERAL		0x01

#define JACK_TYPE_UNDEFINED	0x00
#define JACK_TYPE_EMBEDDED		0x01
#define JACK_TYPE_EXTERNAL		0x02

#ifdef USB_DESCRIPTORS
struct midi_header_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdMSC;
	uint16_t wTotalLength;
} __attribute__((packed));
#define HEADER_SIZE 7

struct midi_jack_in_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bJackType;
	uint8_t bJackID;
	uint8_t iJack;
} __attribute__((packed));
#define JACK_IN_SIZE sizeof(struct midi_jack_in_descriptor)

struct midi_element_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bElementID;
	uint8_t bNrInputPins;
	/* TODO: here goes source pins */
	uint8_t bNrOutputPins;
	uint8_t bInTerminalLink;
	uint8_t bOutTerminalLink;
	uint8_t bElCapsSize;
	uint8_t bmElementCaps;
	uint8_t iElement;
} __attribute__((packed));
#define ELEMENT_SIZE sizeof(struct midi_element_descriptor)
#define ELEMENT_CAPS_SIZE 1

#define CAP_CUSTOM_UNDEFINED		(1 << 0)
#define CAP_MIDI_CLOCK				(1 << 1)
#define CAP_MIDI_TIME_CODE			(1 << 2)
#define CAP_MIDI_MACHINE_CONTROL	(1 << 3)
#define CAP_GM1						(1 << 4)
#define CAP_GM2						(1 << 5)
#define CAP_GS						(1 << 6)
#define CAP_XG						(1 << 7)
#define CAP_EFX						(1 << 8)
#define CAP_MIDI_PATCH_BAY			(1 << 9)
#define CAP_DLS1					(1 << 10)
#define CAP_DLS2					(1 << 11)

struct midi_element_source_pin {
	uint8_t baSourceID;
	uint8_t baSourcePin;
} __attribute__((packed));

struct midi_endpoint_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bNumEmbMIDIJack;
} __attribute__((packed));
#define ENDPOINT_SIZE sizeof(struct midi_endpoint_descriptor)

static const struct {
	struct midi_header_descriptor header;
	struct midi_jack_in_descriptor jack_in_0; 
	//struct midi_element_descriptor element_0;
} __attribute__((packed)) midi_functional_descriptors = {
	.header = {
		.bFunctionLength = HEADER_SIZE,
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = DT_HEADER,
		.bcdMSC = 0x0100,
		.wTotalLength = HEADER_SIZE +
			JACK_IN_SIZE,
	},
	.jack_in_0 = {
		.bLength = JACK_IN_SIZE,
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = DT_IN_JACK,
		.bJackType = JACK_TYPE_EMBEDDED,
		.bJackID = 1,
		.iJack = USBSTR_IDX(JACK_IN_0),
	},
};

static const struct {
	struct midi_endpoint_descriptor bulk_0;
	uint8_t bulk_0_jacks[1];
} __attribute__((packed)) midi_bulk0_extra_descriptors = {
	.bulk_0 = {
		.bLength = sizeof(midi_bulk0_extra_descriptors),
		.bDescriptorType = CS_ENDPOINT,
		.bDescriptorSubtype = DT_ENDP_MS_GENERAL,
		.bNumEmbMIDIJack = 1,
	},
	.bulk_0_jacks = {1},
};

static const struct usb_endpoint_descriptor midi_endp[] = {
{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = ENDP_DIR_IN | 0x1,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 0,

	.extra = &midi_bulk0_extra_descriptors,
	.extralen = sizeof(midi_bulk0_extra_descriptors),
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
