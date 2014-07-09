#include "common.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>

#define DELTA 10

static void adc_setup(void)
{
	//ADC
	rcc_periph_clock_enable(RCC_ADC12);
	rcc_periph_clock_enable(RCC_GPIOA);
	//ADC
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	adc_off(ADC1);
	adc_set_clk_prescale(ADC_CCR_CKMODE_DIV2);
	adc_set_single_conversion_mode(ADC1);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_sample_time(ADC1, 2, ADC_SMPR1_SMP_1DOT5CYC);
	uint8_t channels[16];
	channels[0] = 2;
	adc_set_regular_sequence(ADC1, 1, channels);
	adc_set_resolution(ADC1, ADC_CFGR_RES_12_BIT);
	adc_power_on(ADC1);
	vTaskDelay(50);
}

extern void usb_send_int(int value);

void task_ir_ranger(void *arg)
{
	(void)arg;
	uint16_t v, v_old;

	adc_setup();
	vTaskDelay(500);
	while (1) {
		gpio_toggle(GPIOE, GPIO9);	/* LED on/off */
		adc_start_conversion_regular(ADC1);
		while (! adc_eoc(ADC1))
			taskYIELD();
		v = adc_read_regular(ADC1);
		if (v < 300)
			v = 0;
		if (v > v_old + DELTA ||
			v < v_old - DELTA) {
			v_old = v;
			cdc_send_int(v);
		}
		vTaskDelay(12);
	}
}
