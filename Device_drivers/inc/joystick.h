/**
*@file joystick.h
*@brief provide functions for interfacing with joystick
*
*This header file provide functions for interfacing with joystick.
*The joystick communicate its position (in X axis & Y axis) with the MCU through 2 ADC channel.
*
*@author Tran Thanh Nhan
*@date 27/08/2019
*/

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "stm32f407xx.h"                  // Device header
#include "../../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_adc.h"

/***********************************************************************
Macro definition
***********************************************************************/

/*
*X axis and Y axis position threshold (in 12 bits digital number)
*/
#define X_POS_THRES_L 2000
#define X_POS_THRES_H 2150
#define Y_POS_THRES_L 1925
#define Y_POS_THRES_H 2075

/*
*X axis and Y axis direction
*/
#define X_DIR_LEFT -1
#define X_DIR_RIGHT 1
#define X_DIR_CENTER 0
#define Y_DIR_UP 1
#define Y_DIR_DOWN -1
#define Y_DIR_CENTER 0
/*
*Joystick direction
*/
#define JS_DIR_LEFT_UP 1
#define JS_DIR_LEFT_DOWN 2
#define JS_DIR_LEFT 3
#define JS_DIR_RIGHT_UP 4
#define JS_DIR_RIGHT_DOWN 5
#define JS_DIR_RIGHT 6
#define JS_DIR_UP 7
#define JS_DIR_DOWN 8
#define JS_DIR_CENTERED 0

/***********************************************************************
Function prototype
***********************************************************************/

/**
*@brief Initilize joystick
*@param Pointer to ADCx peripheral (x = 1,2,3)
*@param X axis ADC channel
*@param Y axis ADC channel
*@return none
*/
void joystick_init(ADC_TypeDef *ADCxPtr, uint8_t X_axis_ADC_channel, uint8_t Y_axis_ADC_channel);

/**
*@brief Deinitilize joystick
*@param Pointer to ADCx peripheral (x = 1,2,3)
*@return none
*/
void joystick_deinit(ADC_TypeDef *ADCxPtr);

/**
*@brief Read joystick direction
*
*This read 12 bits digital number from X & Y axis ADC channel then determine the direction of joystick.
*
*@param Pointer to ADCx peripheral (x = 1,2,3)
*@param X axis ADC channel
*@param Y axis ADC channel
*@return Indicator of joystick direction
*/
uint8_t joystick_read_direction(ADC_TypeDef *ADCxPtr, uint8_t X_axis_ADC_channel, uint8_t Y_axis_ADC_channel);
#endif
