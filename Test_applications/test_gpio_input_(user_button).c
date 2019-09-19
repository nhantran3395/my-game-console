#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"

void delay (void)
{
	for (int i = 0;i < 500000;i++){
	}
}

int main (void)
{
	/*initilize led on PD12*/
	GPIO_Pin_config_t GPIO_PD12_config={.pinNumber=GPIO_PIN_NO_12,.mode=GPIO_MODE_OUT,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_PD12_Handle = {GPIOD,GPIO_PD12_config};
	GPIO_init(&GPIO_PD12_Handle);
	
	/*initilize user button on PA0*/
	GPIO_Pin_config_t GPIO_PA0_config = {.pinNumber=GPIO_PIN_NO_0,.mode=GPIO_MODE_IN,.puPdr=GPIO_PDR};
	GPIO_Handle_t	GPIO_PA0_handle = {GPIOA,GPIO_PA0_config};
	GPIO_init(&GPIO_PA0_handle);
	
	while(1){
		if(GPIO_read_pin(GPIOA,GPIO_PIN_NO_0)){
			GPIO_write_pin(GPIOD,12,SET);
		}else{
			GPIO_write_pin(GPIOD,12,CLEAR);
		}
	}
}