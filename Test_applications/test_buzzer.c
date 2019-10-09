/**
*@file test_buzzer.c
*@brief test buzzer driver by playing a part of "Small World"
*
*This program play a part of "Small world", using developed driver for buzzer (see buzzer.h for details). 
*Purpose is to test buzzer driver functions.
*Input signal to buzzer is variable frequence sinewave generated using onboard DAC and TIMERs.
*The frequency of piano notes used in this is defined in "notes.h".
*
*@author Tran Thanh Nhan
*@date 21/08/2019
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include "../../Miscellaneous/inc/notes.h"
#include "../Device_drivers/inc/buzzer.h"

int main (void)
{
//	RCC_set_SYSCLK_PLL_84_MHz();
	/*initilize led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*initilize user button on PA0*/
	button_init(GPIOA,GPIO_PIN_NO_0,GPIO_NO_PUPDR);
	
	buzzer_init(DAC_CHANNEL_1);
	
	while(1){
		if(button_read(GPIOA,GPIO_PIN_NO_0)){
			led_on(GPIOD,GPIO_PIN_NO_12);
			buzzer_play_sound(noteG5, HN+QN);
			buzzer_play_sound(noteG5, QN);
			buzzer_play_sound(noteB5, HN);
			buzzer_play_sound(noteG5, HN);
			buzzer_play_sound(noteA5, HN+QN);
			buzzer_play_sound(noteA5, QN);
			buzzer_play_sound(noteA5, HN+QN);
			buzzer_play_sound(Rest, QN);
			buzzer_play_sound(noteA5, HN+QN);
			buzzer_play_sound(noteA5, QN);
			buzzer_play_sound(noteC6, HN);
			buzzer_play_sound(noteA5, HN);
			buzzer_play_sound(noteB5, HN+QN);
			buzzer_play_sound(noteB5, QN);
			buzzer_play_sound(noteB5, HN+QN);
			buzzer_play_sound(Rest, QN);
			buzzer_play_sound(noteB5, HN+QN);
			buzzer_play_sound(noteB5, QN);
			buzzer_play_sound(noteD6, HN);
			buzzer_play_sound(noteB5, HN);
			buzzer_play_sound(noteC6, HN+QN);
			buzzer_play_sound(noteC6, QN);
			buzzer_play_sound(noteC6, HN);
			buzzer_play_sound(noteB5, QN);
			buzzer_play_sound(noteA5, QN);
			buzzer_play_sound(noteD5, WN);
			buzzer_play_sound(noteFs5, WN);
			buzzer_play_sound(noteG5, WN);
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}
