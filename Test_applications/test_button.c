#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"

void delay (void)
{
	for (int i = 0;i < 500000;i++){
	}
}

int main (void)
{
	/*initilize led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*initilize user button on PA0*/
	button_init(GPIOA,GPIO_PIN_NO_0,GPIO_NO_PUPDR);
	
	while(1){
		if(button_read(GPIOA,GPIO_PIN_NO_0)){
			led_on(GPIOD,GPIO_PIN_NO_12);
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}