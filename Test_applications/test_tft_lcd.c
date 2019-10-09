/**
*@brief test ili9341 driver library 's functions
*
*@author Tran Thanh Nhan
*@date 04/09/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*TFT LCD breakout board CLK 		- 	PB13
*TFT LCD breakout board MISO	-		PB14
*TFT LCD breakout board MOSI	-		PB15
*TFT LCD breakout board CS 			- 	PB12
*TFT LCD breakout board D/C			-		PB11
*TFT LCD breakout board RST		-		PB10
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Device_drivers/inc/ili9341.h"
#include "../Miscellaneous/inc/bitmap_byte_array.h"
#include <stdlib.h>

int main (void)
{
//	RCC_set_SYSCLK_PLL_84_MHz();
	ILI9341_init();
	ILI9341_rotate(ILI9341_orientation_landscape_2);
	ILI9341_fill_display(ILI9341_ORANGE);
	ILI9341_draw_filled_rectangle(0,0,160,120,ILI9341_BLUE);
}
