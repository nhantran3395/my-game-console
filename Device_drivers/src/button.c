/**
*@file button.h
*@brief provide functions for interfacing with button
*
*This header file provide functions for interfacing with button.
*
*@author Tran Thanh Nhan
*@date 16/08/2019
*/

#include "../inc/button.h"

void button_init (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber, uint8_t puPdr)
{
	GPIO_Pin_config_t GPIO_button_config = {.pinNumber=pinNumber,.mode=GPIO_MODE_IN,.puPdr=puPdr};
	GPIO_Handle_t	GPIO_button_handle = {GPIOxPtr,GPIO_button_config};
	GPIO_init(&GPIO_button_handle);
}	

uint8_t button_read (GPIO_TypeDef *GPIOxPtr,uint8_t pinNumber)
{
	return GPIO_read_pin(GPIOxPtr,pinNumber);
}

