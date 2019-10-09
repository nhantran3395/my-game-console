/**
*@file speaker.h
*@brief provide functions for interfacing with speaker
*
*This header file provide functions for interfacing with speaker.
*Speaker is driven with STM32F4 Discovery board 's on-board DAC (user can select channel 1 or channel 2).
*Timer 7 is used for creating time interval between sound samples.
*
*@author Tran Thanh Nhan
*@date 21/08/2019
*/

/*
 *@version 1.1
 *date 21/09/2019
 *change library name to speaker
 *add timer selection to initialize function
 *remove duration from play sound function
 */

#ifndef SPEAKER_H
#define SPEAKER_H

#include "../../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_dac.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_timer.h"

#define SPEAKER_USE_TIMER7			TRUE
#define SPEAKER_TIMER				TIM7
#define SPEAKER_TIMER_IRQ_NUM		IRQ_TIM7

/**
*@brief 	Initialize speaker (driven by on-board DAC)
*
*@param 	DAC channel 1 or DAC channel 2
*@param 	Timer prescaler value
*@param 	Timer reload value
*@return 	None
*/
void speaker_init (uint8_t DAC_channel, uint16_t timerPrescaler, uint16_t timerReload);

/**
*@brief 	Generate sound
*
*This read sound 's samples at frequency specified in speaker_init function and output to DAC
*
*@param 	Array of samples of sound to play
*@return 	None
*/
void speaker_play_sound (const uint16_t *SoundPtr, const uint16_t size);

/**
*@brief 	Stop sound
*
*@param 	None
*@return 	None
*/
void speaker_stop_sound (void);

#endif
