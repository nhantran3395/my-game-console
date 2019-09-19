/**
*@file led.h
*@brief provide functions for interfacing with led
*
*This header file provide functions for interfacing with led.
*
*@author Tran Thanh Nhan
*@date 13/08/2019
*/

#ifndef LED_H
#define LED_H

#include "stm32f407xx.h"                  // Device header
#include "../../Peripheral_drivers/inc/stm32f407xx_gpio.h"

/**
*@brief Initilize led 
*@param Pointer to base address of GPIO port x registers
*@param Pin number
*@return none
*/
void led_init (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber);

/**
*@brief Turn on led
*@param Pointer to base address of GPIO port x registers
*@param Pin number
*@return none
*/
void led_on (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber);

/**
*@brief Turn off led
*@param Pointer to base address of GPIO port x registers
*@param Pin number
*@return none
*/
void led_off (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber);

/**
*@brief Toggle led
*@param Pointer to base address of GPIO port x registers
*@param Pin number
*@return none
*/
void led_toggle (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber);
#endif
