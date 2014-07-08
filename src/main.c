#include "common.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

static void systick_setup(void)
{
	systick_set_frequency(configTICK_RATE_HZ, configCPU_CLOCK_HZ);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();
	/* this done last */
	systick_interrupt_enable();
}

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12 | GPIO13 |
			GPIO14 | GPIO15);
}

static void task_blink(void *arg)
{
	(void)arg;
	while (1) {
		gpio_toggle(GPIOE, GPIO8);	/* LED on/off */
		vTaskDelay(500);
	}
}

void vApplicationTickHook(void)
{
}

void vApplicationIdleHook(void)
{
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *name )
{
	(void)name;
	(void)task;

	taskDISABLE_INTERRUPTS();
	for(;;);
}

extern void task_usb(void*);
extern void task_ir_ranger(void*);
int main(void)
{
	rcc_clock_setup_hsi(&hsi_8mhz[CLOCK_48MHZ]);
	gpio_setup();
	systick_setup();
	
	xTaskCreate(task_usb,		"usb",			4096,	NULL,	3,	NULL);
	xTaskCreate(task_blink,		"blink",		256,	NULL,	4,	NULL);
	xTaskCreate(task_ir_ranger,	"ir_ranger",	1024,	NULL,	3,	NULL);
	vTaskStartScheduler();
	for(;;);
}
