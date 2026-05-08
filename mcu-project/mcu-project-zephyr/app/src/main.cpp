#include <comms_thread.hpp>
#include <adc_thread.hpp>
#include <calculation_thread.hpp>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>

// Device tree setup
#define LED0_NODE DT_ALIAS(led0)
#define COMMS_UART DT_ALIAS(comms_uart)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct device* external_uart = DEVICE_DT_GET(COMMS_UART);
static const struct adc_dt_spec potentiometer = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

// Message queue setup between threads
// (10 items, 2 bytes each, aligned to 2-byte boundaries)
K_MSGQ_DEFINE(raw_adc_queue, sizeof(uint16_t), 10, 2);
K_MSGQ_DEFINE(calculated_data_queue, sizeof(uint16_t), 10, 2);

int main(void)
{
	if (!gpio_is_ready_dt(&led))
    {
		return 0;
	}
	if(gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0)
	{
		return 0;
	}

	if(!device_is_ready(external_uart))
	{
		printk("UART device not ready\n");
		return 0;
	}
	if(!adc_is_ready_dt(&potentiometer))
	{
		printk("Potentiometer ADC device not ready\n");
        return 0;
    }
	
	comms::comms_thread_init(external_uart);
	adc::adc_thread_init(&potentiometer);
	calculation::calculation_thread_init();

    bool led_state = false;
    while(1)
    {
        if(gpio_pin_set_dt(&led, led_state) < 0)
		{
			printk("Error: Could not toggle pin\n");
		}
        led_state = !led_state;
        k_msleep(1000);
    }
}