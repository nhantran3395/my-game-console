#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"

void delay (void)
{
	for (int i = 0;i < 500000;i++){
	}
}

void EXTI9_5_IRQHandler (void)
{
	GPIO_Intrpt_handler(GPIO_PIN_NO_5);
	GPIO_toggle_pin(GPIOD,GPIO_PIN_NO_12);
}

int main (void)
{
	/*initilize led on PD12*/
	GPIO_Pin_config_t GPIO_PD12_config={.pinNumber=GPIO_PIN_NO_12,.mode=GPIO_MODE_OUT,.speed=GPIO_OUTPUT_LOW_SPEED,.outType=GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_PD12_Handle = {GPIOD,GPIO_PD12_config};
	GPIO_Handle_t *ptrGPIO_PD12_Handle = &GPIO_PD12_Handle;
	GPIO_CLK_ctr(GPIOD,ENABLE);
	GPIO_init(ptrGPIO_PD12_Handle);
	
	/*initilize falling edge interrupt on PD5*/
	GPIO_Pin_config_t GPIO_PD5_config={.pinNumber=GPIO_PIN_NO_5,.mode=GPIO_MODE_INTRPT_FE,.puPdr=GPIO_PU};
	GPIO_Handle_t GPIO_PD5_Handle = {GPIOD,GPIO_PD5_config};
	GPIO_Handle_t *ptrGPIO_PD5_Handle = &GPIO_PD5_Handle;
	GPIO_init(ptrGPIO_PD5_Handle);
	
	GPIO_Intrpt_ctrl(IRQ_EXTI9_5,ENABLE);
	GPIO_Intrpt_priority_config(IRQ_EXTI9_5,15);
	
	while(1){
		/*do something*/
	}
}