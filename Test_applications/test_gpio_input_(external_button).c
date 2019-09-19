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
	GPIO_Handle_t *ptrGPIO_PD12_Handle = &GPIO_PD12_Handle;
	GPIO_CLK_ctr(GPIOD,ENABLE);
	GPIO_init(ptrGPIO_PD12_Handle);
	
	/*initilize external button on PB0*/
	GPIO_Pin_config_t GPIO_PB0_config={.pinNumber=GPIO_PIN_NO_0,.mode=GPIO_MODE_IN,.puPdr=GPIO_PU};
	GPIO_Handle_t GPIO_PB0_Handle = {GPIOB,GPIO_PB0_config};
	GPIO_Handle_t *ptrGPIO_PB0_Handle = &GPIO_PB0_Handle;
	GPIO_CLK_ctr(GPIOB,ENABLE);
	GPIO_init(ptrGPIO_PB0_Handle);
	
	while(1){
		if(GPIO_read_pin(GPIOB,0)==0){
			GPIO_write_pin(GPIOD,12,SET);
		}else{
			GPIO_write_pin(GPIOD,12,CLEAR);
		}
	}
}