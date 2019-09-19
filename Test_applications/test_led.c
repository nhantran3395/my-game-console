#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Device_drivers/inc/led.h"

void delay (void)
{
	for (int i = 0;i < 500000;i++){
	}
}

int main (void)
{
	/*initilize green led on PD12, red led on PD14, blue led on PD15*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	led_init(GPIOD,GPIO_PIN_NO_14);
	led_init(GPIOD,GPIO_PIN_NO_15);
	
	while(1){
		led_on(GPIOD,GPIO_PIN_NO_12);
		led_on(GPIOD,GPIO_PIN_NO_14);
		led_on(GPIOD,GPIO_PIN_NO_15);
		delay();
		led_off(GPIOD,GPIO_PIN_NO_12);
		led_off(GPIOD,GPIO_PIN_NO_14);
		led_off(GPIOD,GPIO_PIN_NO_15);
		delay();
	}
}