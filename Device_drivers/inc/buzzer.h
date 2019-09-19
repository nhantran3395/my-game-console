/**
*@file buzzer.h
*@brief provide functions for interfacing with buzzer
*
*This header file provide functions for interfacing with buzzer.
*Buzzer is driven with sinewave signal from DAC output (user can select channel 1 or channel 2).
*Timer 6 is used to create time step between samples (Reload value of timer 6 is varied to generate signal with different frequencies) 
*Timer 7 is used to create sound duration (Reload value of timer 7 is varied to generate different sound duaration)
*
*@note Due to the fact that prescaler value of TIM6 and TIM7 is consider fixed (for simplicity), possible range of sound frequency is 24 Hz - 160 Khz,
*possible range of sound duration is 0.625 milisecond - 40.96 second					
*
*@author Tran Thanh Nhan
*@date 21/08/2019
*/

#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f407xx.h"                  // Device header
#include "../../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_dac.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_timer.h"

#define SAMPLE_NUM 10
#define TIM6_PRESCALE_VAL 99
#define TIM7_PRESCALE_VAL 9999

/**
*@brief Initialize buzzer (driven by DAC channer 1 or 2)
*
*This initilize GPIO PA4 (or PA5) as analog pin,
*then initialize DAC channel 1 (or channel 2) with 12 bits resolution, right alligned, output buffer enabled.
*Initilize and enable interrupt for TIM6 and TIM7
*
*@param DAC channel
*@return none
*/
void buzzer_init (uint8_t DAC_channel);

/**
*@brief Generate sound with given frequency for given duration
*
*This calculate and set reload value for TIM6 and TIM7 based on given frequency and duaration,
*then start TIM6 and TIM7
*
*@param Frequency of sound to be genereted (in Hz, possible range is 24 Hz - 160 Khz)
*@param Sound duration (in milisecond, possible range is 0.625 milisecond - 40.96 second)	
*@return none
*/
void buzzer_play_sound (uint32_t freq, uint32_t duration);

/**
*@brief Stop sound
*
*@param none
*@return none
*/
void buzzer_stop_sound (void);

#endif