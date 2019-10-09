/**
*@file led.h
*@brief provide functions for interfacing with led
*
*This source file provide functions for interfacing with led.
*
*@author Tran Thanh Nhan
*@date 13/08/2019
*/

#include "../inc/led.h"

void led_init (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber)
{
	GPIO_Pin_config_t GPIO_led_config={.pinNumber=pinNumber,.mode=GPIO_MODE_OUT,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_led_Handle = {GPIOxPtr,GPIO_led_config};
	GPIO_init(&GPIO_led_Handle);
}

void led_on (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber)
{
	GPIO_write_pin(GPIOxPtr, pinNumber,SET);
}

void led_off (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber)
{
	GPIO_write_pin(GPIOxPtr, pinNumber,CLEAR);
}

void led_toggle (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber)
{
	GPIO_toggle_pin(GPIOxPtr, pinNumber);
}
