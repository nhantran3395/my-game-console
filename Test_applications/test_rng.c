/**
*@brief 		Test random number generator (RNG) 's driver library functions on STM32F407 microcontroller  
*
* 							This generate a random 32-bits number using the onboard RNG. The random value will then be sent through UART and display on PC 
*
*@note 		RNG 's clock (RNG_CLK) must satisfy the following condition: RNG_CLK >= HCLK/16
*								RNG_CLK is derived from PLL clock: PLL_CLK/PLLQ (PLLQ is division factor which value can be program in RCC_PLLCFGR)
*								In order for RNG to be correctly configured, user need to call RCC_set_SYSCLK_PLL_84_MHz before enabling RNG
*
*@author 	Tran Thanh Nhan
*@date 		10/09/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*UART2 TX	- PA2
*UART2 RX	- PA3
*/

#include "stm32f4xx.h"                  // Device header
#include	"../Peripheral_drivers/inc/stm32f407xx_rng.h"
#include	"../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include	"../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
	
void delay (volatile uint32_t delay)
{
	for(;delay !=0 ;delay--){}		
}

UART_Handle_t *UART2HandlePtr = NULL;

int main (void)
{
	uint32_t random_num = 0;
	char str[30];
	
	RCC_set_SYSCLK_PLL_84_MHz ();

	UART2HandlePtr = UART_general_init(USART2,UART_pins_pack_1,UART_BDR_9600,UART_STB_1,UART_WRDLEN_8_DT_BITS,UART_TX_RX,UART_NO_PARCTRL,UART_NO_FLOWCTRL);
	
	RNG_init ();
	while(1){
		random_num = RNG_get();
		sprintf(str,"Random value: %u\n\r",random_num);
		UART_send(UART2HandlePtr,(uint8_t*)str,strlen(str))	;
		delay(500000);
	}
}
