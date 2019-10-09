/*
 * @file 	test_speaker.c
 *
 * @brief 	Confirm the working of speaker driver library
 *
 * @date	23/09/2019
 * @author	Tran Thanh Nhan
 */

#include "stm32f407xx.h"
#include "../Device_drivers/inc/speaker.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Miscellaneous/inc/rocket_launch.h"

int main (void)
{
	RCC_set_SYSCLK_PLL_84_MHz();
	speaker_init(DAC_CHANNEL_1,9,679);
	led_init(GPIOD,GPIO_PIN_NO_12);
	led_init(GPIOD,GPIO_PIN_NO_13);
	button_init(GPIOA,GPIO_PIN_NO_0,GPIO_PDR);

	while(1)
	{
		if(button_read(GPIOA,GPIO_PIN_NO_0)){
			led_on(GPIOD,GPIO_PIN_NO_12);
			speaker_play_sound(rocket_launch,(sizeof(rocket_launch)/sizeof(rocket_launch[0])));
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}
