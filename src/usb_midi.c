#include "usb_midi.h"

static void midi_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)usbd_dev;
	(void)ep;
}

void midi_set_config(usbd_device *usbd_dev)
{
	usbd_ep_setup(usbd_dev, midi_endp_in(1), USB_ENDPOINT_ATTR_BULK, 64, midi_data_rx_cb);
}

void midi_msg(uint8_t cable, midi_event_type type, uint8_t chan, uint8_t data1, uint8_t data2)
{
	uint8_t buf[4];
	buf[0] = ((cable & 0xf) << 4) | (type & 0xf);
	buf[1] = ((type & 0xf) << 4) | (chan & 0xf);
	buf[2] = data1;
	buf[3] = data2;
	if (!usb)
		return;
	usbd_ep_write_packet(usb, midi_endp_in(1), buf, sizeof(buf));
}
