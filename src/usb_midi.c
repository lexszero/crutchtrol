#include "usb_midi.h"

static void midi_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)usbd_dev;
	(void)ep;
}

void midi_set_config(usbd_device *usbd_dev)
{
	usbd_ep_setup(usbd_dev, 0x01, USB_MIDI_ENDP_DIR_IN | 0, 64, midi_data_rx_cb);
}
