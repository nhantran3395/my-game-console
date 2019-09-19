/**
*@file stm32f407xx_gpio.h
*@brief provide functions for interfacing with GPIO pin
*
*This header file provide functions for interfacing with GPIO pins on stm32f407xx MCUs.
*
*@author Tran Thanh Nhan
*@date 23/07/2019
*/

#ifndef STM32F407XX_GPIO_H
#define STM32F407XX_GPIO_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include <stdint.h>

/*
*@GPIO_PIN_NO
*GPIO pin number
*/
#define GPIO_PIN_NO_0	0
#define GPIO_PIN_NO_1	1
#define GPIO_PIN_NO_2	2
#define GPIO_PIN_NO_3	3
#define GPIO_PIN_NO_4	4
#define GPIO_PIN_NO_5	5
#define GPIO_PIN_NO_6	6
#define GPIO_PIN_NO_7	7
#define GPIO_PIN_NO_8	8
#define GPIO_PIN_NO_9	9
#define GPIO_PIN_NO_10	10
#define GPIO_PIN_NO_11	11
#define GPIO_PIN_NO_12	12
#define GPIO_PIN_NO_13	13
#define GPIO_PIN_NO_14	14
#define GPIO_PIN_NO_15	15

/*
*@GPIO_MODE
*GPIO pin possible modes
*/
#define GPIO_MODE_IN 0
#define GPIO_MODE_OUT 1
#define GPIO_MODE_ALTFN 2
#define GPIO_MODE_ANL 3
#define GPIO_MODE_INTRPT_FE	4	//Interrupt, falling edge
#define GPIO_MODE_INTRPT_RE	5	//Interrupt, raising edge
#define GPIO_MODE_INTRPT_RFE	6	//Interrupt, both edge

/*
*@GPIO_OUTPUT_MODE
*GPIO pin possible output modes
*/
#define GPIO_OUTPUT_TYPE_PP 0 //Push-pull mode
#define GPIO_OUTPUT_TYPE_OD 1	//Open drain mode

/*
*@GPIO_OUTPUT_SPEED
*GPIO output speed
*/
#define GPIO_OUTPUT_LOW_SPEED 0
#define	GPIO_OUTPUT_MEDIUM_SPEED 1
#define GPIO_OUTPUT_HIGH_SPEED 2
#define GPIO_OUTPUT_VERY_HIGH_SPEED 3

/*
*GPIO_PUPDR_MODE
*GPIO pin input modes
*/
#define GPIO_NO_PUPDR 0
#define GPIO_PU 1
#define GPIO_PDR 2

typedef struct
{
	uint8_t pinNumber; /*possible value from @GPIO_PIN_NO*/
	uint8_t mode;	/*posible value from @GPIO_MODE*/
	uint8_t speed;	/*posible value from @GPIO_OUTPUT_SPEED*/
	uint8_t outType;	/*posible value from @GPIO_OUTPUT_TYPE*/
	uint8_t puPdr;	/*posible value from @GPIO_PUPDR_MODE*/
	uint8_t altFunc;
}GPIO_Pin_config_t;

typedef struct 
{
	GPIO_TypeDef *GPIOxPtr; /*This hold the base address of the GPIO port where the pin belong*/
	GPIO_Pin_config_t GPIO_Pin_config; /*This hold configuration of GPIO pin*/
}GPIO_Handle_t;

/**
*@brief GPIO port clock setup 
*@param Pointer to base address of GPIO port x registers
*@param Enable or disable action
*@return none
*/
void GPIO_CLK_ctr (GPIO_TypeDef *GPIOxPtr, uint8_t enOrDis);

/**
*@brief Initialize GPIO pin
*@param Pointer to GPIO_Handle struct
*@return none
*/
void GPIO_init (GPIO_Handle_t *GPIOxHandlePtr);

/**
*@brief Initialize corresponded GPIO pin directly using given parameter
*@param Pointer to GPIO port x  
*@return none
*/
void GPIO_init_direct (GPIO_TypeDef *GPIOxPtr,uint8_t pinNumber,uint8_t mode,uint8_t speed, uint8_t outType, uint8_t puPdr, uint8_t altFunc);

/**
*@brief Deinitilize GPIO pin
*@param Pointer to base address of GPIO port x registers
*@return none
*/
void GPIO_deinit (GPIO_TypeDef *GPIOxPtr);

/**
*@brief Read data from GPIO input pin
*@param Pointer to base address of GPIO port x registers
*@param GPIO pin number
*@return Data from GPIO input pin
*/
uint8_t GPIO_read_pin (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber);

/**
*@brief Read data from GPIO input port
*@param Pointer to base address of GPIO port x registers
*@return Data from GPIO port
*/
uint16_t GPIO_read_port (GPIO_TypeDef *GPIOxPtr);

/**
*@brief Write data to GPIO output pin
*@param Pointer to base address of GPIO port x registers
*@param GPIO pin number
*@param Set or clear action
*@return none
*/
void GPIO_write_pin (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber, uint8_t setOrClear);

/**
*@brief Write data to GPIO output port
*@param Pointer to base address of GPIO port x registers
*@param Set or clear action
*@return none
*/
void GPIO_write_port (GPIO_TypeDef *GPIOxPtr,uint16_t value);

/**
*@brief Toggle GPIO output pin
*@param Pointer to base address of GPIO port x registers
*@param GPIO pin number
*@return none
*/
void GPIO_toggle_pin (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber);

/**
*@brief Config interrupt priority
*@param IRQ number
*@param Priority
*@return none
*/
void GPIO_Intrpt_priority_config (uint8_t IRQnumber, uint8_t IRQpriority);

/**
*@brief Enable or disable GPIO pin 's interrupt 
*@param IRQ number
*@param Enable or disable action
*@return none
*/
void GPIO_Intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis);

/**
*@brief Handler for GPIO pin 's interrupt
*@param GPIO pin number
*@return none
*/
void GPIO_Intrpt_handler (uint8_t pinNumber);

#endif 
