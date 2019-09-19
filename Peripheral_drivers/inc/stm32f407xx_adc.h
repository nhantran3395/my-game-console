/**
*@file stm32f407xx_adc.h
*@brief provide APIs for interfacing with ADCs on stm32f407xx MCUs.
*
*This header file provide APIs for interfacing with ADCs on stm32f407xx MCUs.
*
*@note This library only support the following configurations and features:
*	number of conversion: 1
*	resolution: 12 bits
*	conversion mode: single
*	read value in busy-wait method
*
*@author Tran Thanh Nhan
*@date 26/08/2019
*/

#ifndef STM32F407XX_ADC_H
#define STM32F407XX_ADC_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include "stm32f407xx_gpio.h"
#include <stdint.h>
#include <stdlib.h>

/***********************************************************************
Macro definition
***********************************************************************/

/*
*@ADC_CHANNEL
*ADC channel selection
*/
#define ADC_CHANNEL_0	0 
#define ADC_CHANNEL_1	1
#define ADC_CHANNEL_2	2
#define ADC_CHANNEL_3	3
#define ADC_CHANNEL_4	4
#define ADC_CHANNEL_5	5
#define ADC_CHANNEL_6	6
#define ADC_CHANNEL_7	7
#define ADC_CHANNEL_8	8
#define ADC_CHANNEL_9	9
#define ADC_CHANNEL_10	10
#define ADC_CHANNEL_11	11
#define ADC_CHANNEL_12	12
#define ADC_CHANNEL_13	13
#define ADC_CHANNEL_14	14
#define ADC_CHANNEL_15	15

/*
*@ADC_RESOLUTION
*ADC resolution selection
*/
#define ADC_RES_12_bits	0
#define ADC_RES_10_bits	1
#define ADC_RES_8_bits	2
#define ADC_RES_6_bits	3

/*
*@ADC_CVSMODE
*ADC conversion mode selection
*/
#define ADC_CVSMODE_SINGLE 0
#define ADC_CVSMODE_CONT 1

/***********************************************************************
ADC structure definition
***********************************************************************/

typedef struct{
	uint8_t numOfConversion;
	uint8_t resolution;	/*refer to @ADC_RESOLUTION for possible value*/
	uint8_t conversionMode;	/*refer to @ADC_CVSMODE for possible value*/
}ADC_Config_t;

typedef struct{
	ADC_TypeDef *ADCxPtr;
	ADC_Config_t *ADCxConfigPtr;
}ADC_Handle_t;

/***********************************************************************
ADC driver APIs prototype
***********************************************************************/

/**
*@brief ADC peripheral clock enable/disable
*@param Pointer to ADCx 's base address
*@param Enable or disable action
*@return none
*/
void ADC_CLK_ctr(ADC_TypeDef *ADCxPtr, uint8_t enOrDis);

/**
*@brief ADC peripheral on/off
*
*Set or clear ADON bit in CR2 register to turn on/off ADC. 
*
*@param Pointer to ADCx 's base address
*@param On or off 
*@return none
*/
void ADC_ctr(ADC_TypeDef *ADCxPtr, uint8_t onOrOff);

/**
*@brief Initialize ADC
*
*This perform the following operations:
*	enable clock for ADCx peripheral (x is 1 or 2 or 3)
*	turn off ADCx for initialization
*	set ADCx resolution to 12 bits
*	set conversion mode to single
*	select EOC as indicating end of each regular conversion
*	
*@param Pointer to ADC handle struct
*@return none
*/
void ADC_init(ADC_Handle_t *ADCxHandlePtr);

/**
*@brief Initialize ADC channel x (x is any number in range 0 - 15)
*
*This initilize GPIO pin corresponed to ADC channel x and initilize ADCx peripheral
*	
*@param Pointer to ADCx peripheral (x is 1 or 2 or 3)
*@param ADC channel x
*@return none
*/
void ADC_init_channel(ADC_TypeDef *ADCxPtr,uint8_t ADC_channel_x);

/**
*@brief Deinitialize all ADCs 
*@return none
*/
void ADC_deinit(void);

/**
*@brief Read value from ADC channel
*@param Pointer to ADCx 's base address
*@param ADC channel
*@return Value from ADC
*/
uint16_t ADC_read(ADC_TypeDef *ADCxPtr, uint8_t channel);
#endif