/**
*@file stm32f407xx_dac.h
*@brief provide APIs for interfacing with DAC on stm32f407xx MCUs.
*
*This header file provide APIs for interfacing with DAC on stm32f407xx MCUs.
*
*@author Tran Thanh Nhan
*@date 19/08/2019
*/

/*
 *@Version 1.1
 *Date 21/09/2019
 *Add DAC_init_channel function
 */

#ifndef STM32F407XX_DAC_H
#define STM32F407XX_DAC_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include "stm32f407xx_gpio.h"
#include <stdint.h>
#include <stdlib.h>

/***********************************************************************
Macro definition
***********************************************************************/

/*
*@DAC_CHANNEL
*DAC channel selection
*/
#define DAC_CHANNEL_1	0 /*DAC channel 1 on PA4*/
#define DAC_CHANNEL_2	1	/*DAC channel 2 on PA5*/

/*
*@DAC_RESOLUTION
*DAC resolution selection
*/
#define DAC_RES_8_bits	0
#define DAC_RES_12_bits	1

/*
*@DAC_12BITS_ALIGNMENT
*Alignment selection in 12 bits-resolution DAC
*/
#define DAC_ALIGNMENT_LEFT 0
#define DAC_ALIGNMENT_RIGHT 1

/*
*@DAC_TRIGGER_EV
*DAC trigger event selection
*/
#define DAC_TRIGGER_EV_TIM6	0
#define DAC_TRIGGER_EV_TIM8	1
#define DAC_TRIGGER_EV_TIM7	2
#define DAC_TRIGGER_EV_TIM5	3
#define DAC_TRIGGER_EV_TIM2	4
#define DAC_TRIGGER_EV_TIM4	5
#define DAC_TRIGGER_EV_EXTI9	6
#define DAC_TRIGGER_EV_SW	7
#define DAC_NO_TRIGGER_EV	8

/*
*@DAC_OUTPUT_BUFFER
*DAC output buffer enable/disable selection
*/
#define DAC_OBUFFER_EN 0
#define DAC_OBUFFER_DIS 1

/***********************************************************************
DAC structure definition
***********************************************************************/

typedef struct{
	uint8_t channel; /*refer to @DAC_CHANNEL for possible value*/
	uint8_t resolution;	/*refer to @DAC_RESOLUTION for possible value*/
	uint8_t alignment;	/*refer to @DAC_12BITS_ALIGNMENT for possible value*/
	uint8_t triggerEV;	/*refer to @DAC_TRIGGER_EV for possible value*/	
	uint8_t outputBuffer;	/*refer to @DAC_OUTPUT_BUFFER for possible value*/
}DAC_Config_t;

typedef struct{
	DAC_TypeDef *DACxPtr;
	DAC_Config_t *DACxConfigPtr;
}DAC_Handle_t;

/***********************************************************************
DAC driver APIs prototype
***********************************************************************/

/**
*@brief DAC peripheral clock enable/disable
*@param Pointer to base address of DAC peripheral 's registers
*@param Enable or disable action
*@return none
*/
void DAC_CLK_ctr(DAC_TypeDef *DACxPtr, uint8_t enOrDis);

/**
*@brief DAC peripheral enable/disable
*
*Set or clear EN bit in CR register to enable/disable DAC1/DAC2 channel. 
*
*@param Pointer to base address of DAC peripheral 's registers
*@param Enable or disable action
*@return none
*/
void DAC_periph_ctr(DAC_Handle_t *DACxHandlePtr, uint8_t enOrDis);

/**
*@brief Initialize DAC
*@param Pointer to DAC handle struct
*@return none
*/
void DAC_init(DAC_Handle_t *DACxHandlePtr);

/**
*@brief 	Initialize DAC channel (channel 1 or channel 2)
*
*This initilize DAC channel 1 ( or channel 2 ) with following configurations:
*	12-bits resolution
*	Right data alignment
*	No trigger event
*	Output buffer enabled
*
*@param 	Channel
*@return 	None
*/
void DAC_init_channel(uint8_t channel);

/**
*@brief 	Deinitialize DAC
*@param 	Pointer to DAC Handle struct
*@return 	None
*/
void DAC_deinit(DAC_TypeDef *DACxPtr);

/**
*@brief 	Write digital value to DAC for conversion
*@param 	Pointer to DAC handle struct
*@param 	Digital value
*@return 	None
*/
void DAC_write(DAC_Handle_t *DACxHandlePtr, uint16_t digiVal);

#endif
