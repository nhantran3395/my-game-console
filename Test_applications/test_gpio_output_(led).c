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
	
	/*initilize led on PD13*/
	GPIO_Pin_config_t GPIO_PD13_config={.pinNumber=GPIO_PIN_NO_13,.mode=GPIO_MODE_OUT,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_PD13_Handle = {GPIOD,GPIO_PD13_config};
	GPIO_Handle_t *ptrGPIO_PD13_Handle = &GPIO_PD13_Handle;
	GPIO_init(ptrGPIO_PD13_Handle);
	
	/*initilize led on PD14*/
	GPIO_Pin_config_t GPIO_PD14_config={.pinNumber=GPIO_PIN_NO_14,.mode=GPIO_MODE_OUT,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_PD14_Handle = {GPIOD,GPIO_PD14_config};
	GPIO_Handle_t *ptrGPIO_PD14_Handle = &GPIO_PD14_Handle;
	GPIO_init(ptrGPIO_PD14_Handle);
	
	/*initilize led on PD15*/
	GPIO_Pin_config_t GPIO_PD15_config={.pinNumber=GPIO_PIN_NO_15,.mode=GPIO_MODE_OUT,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_PD15_Handle = {GPIOD,GPIO_PD15_config};
	GPIO_Handle_t *ptrGPIO_PD15_Handle = &GPIO_PD15_Handle;
	GPIO_init(ptrGPIO_PD15_Handle);

	while(1){
		GPIO_write_port(GPIOD,0xF000);
		delay();
		GPIO_write_port(GPIOD,0x0000);
		delay();
	}
}