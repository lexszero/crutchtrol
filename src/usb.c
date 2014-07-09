#include "common.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/cdc.h>

#define USB_DESCRIPTORS
#include "usb_cdc.h"
#include "usb_midi.h"

usbd_device *usb;

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

#define X(name, value) value,
static const char *usb_strings[] = {
USB_STRING_TABLE
};
#undef X

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_CDC,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0x5740,
	.bcdDevice = 0x0200,
	.iManufacturer = USBSTR_IDX(MANUFACTURER),
	.iProduct = USBSTR_IDX(PRODUCT),
	.iSerialNumber = USBSTR_IDX(SERIAL),
	.bNumConfigurations = 1,
};

static const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = comm_iface,
}, {
	.num_altsetting = 1,
	.altsetting = data_iface,
}, {
	.num_altsetting = 1,
	.altsetting = midi_iface,
}};

static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 3,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,

	.interface = ifaces,
};

static void usb_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;
	(void)usbd_dev;

	cdc_set_config(usbd_dev);
	midi_set_config(usbd_dev);
}

static void usb_setup(void)
{
	/* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
	rcc_usb_prescale_1();
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USBEN);
	rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_IOPAEN);

	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF14, GPIO11| GPIO12);
}

void task_usb(void *arg)
{
	(void)arg;

	usb_setup();
	usb = usbd_init(&stm32f103_usb_driver, &dev, &config, usb_strings,
			3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usb, usb_set_config);

	vTaskDelay(50);
	
	while (1) {
		gpio_toggle(GPIOE, GPIO10);
		usbd_poll(usb);
		taskYIELD();
	}
}


