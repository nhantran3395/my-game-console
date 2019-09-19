/**
*@file button.h
*@brief provide functions for interfacing with button
*
*This header file provide functions for interfacing with button.
*
*@author Tran Thanh Nhan
*@date 16/08/2019
*/

#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f407xx.h"                  // Device header
#include "../../Peripheral_drivers/inc/stm32f407xx_gpio.h"

void button_init (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber, uint8_t puPdr);	
uint8_t button_read (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber);
#endif