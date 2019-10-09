/**
*@file joystick.c
*@brief provide functions for interfacing with joystick
*
*This implementation file provide functions for interfacing with joystick.
*The joystick communicate its position (in X axis & Y axis) with the MCU through 2 ADC channel.
*
*@author Tran Thanh Nhan
*@date 27/08/2019
*/

#include "../inc/joystick.h"                

/***********************************************************************
Initilize joystick
***********************************************************************/
void joystick_init(ADC_TypeDef *ADCxPtr, uint8_t X_axis_ADC_channel, uint8_t Y_axis_ADC_channel)
{
	ADC_init_channel(ADCxPtr,X_axis_ADC_channel);
	ADC_init_channel(ADCxPtr,Y_axis_ADC_channel);
}

/***********************************************************************
Deinitilize joystick
***********************************************************************/
void joystick_deinit(ADC_TypeDef *ADCxPtr)
{
	ADC_deinit();
}

/***********************************************************************
Read joystick direction
***********************************************************************/
uint8_t joystick_read_direction(ADC_TypeDef *ADCxPtr, uint8_t X_axis_ADC_channel, uint8_t Y_axis_ADC_channel)
{
	int8_t xDir = 0, yDir = 0;
	uint16_t xPos = ADC_read(ADCxPtr,X_axis_ADC_channel);
	uint16_t yPos = ADC_read(ADCxPtr,Y_axis_ADC_channel);
	
	if(xPos > X_POS_THRES_H){
		xDir = X_DIR_RIGHT;
	}else if(xPos < X_POS_THRES_L){
		xDir = X_DIR_LEFT;
	}else{
		xDir = X_DIR_CENTER;
	}
	
	if(yPos > Y_POS_THRES_H){
		yDir = Y_DIR_UP;
	}else if(yPos < Y_POS_THRES_L){
		yDir = Y_DIR_DOWN;
	}else{
		yDir = Y_DIR_CENTER;
	}
	
	if(xDir == X_DIR_LEFT){
		if(yDir == Y_DIR_UP){
			return JS_DIR_LEFT_UP;
		}else if(yDir == Y_DIR_DOWN){
			return JS_DIR_LEFT_DOWN;
		}else if(yDir == Y_DIR_CENTER){
			return JS_DIR_LEFT;
		}
	}
	
	if(xDir == X_DIR_RIGHT){
		if(yDir == Y_DIR_UP){
			return JS_DIR_RIGHT_UP;
		}else if(yDir == Y_DIR_DOWN){
			return JS_DIR_RIGHT_DOWN;
		}else if(yDir == Y_DIR_CENTER){
			return JS_DIR_RIGHT;
		}
	}

	if(xDir == X_DIR_CENTER){
		if(yDir == Y_DIR_UP){
			return JS_DIR_UP;
		}else if(yDir == Y_DIR_DOWN){
			return JS_DIR_DOWN;
		}else if(yDir == Y_DIR_CENTER){
			return JS_DIR_CENTERED;
		}
	}	
}
