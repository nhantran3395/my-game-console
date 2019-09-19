/**
*@brief test SPI send data APIs by sending data from STM32F4xx to Arduino using SPI
*
*This program send data from STM32F4 to Arduino through SPI2 peripheral whenever user button on STM32F4 board is press. Purpose is to test SPI send data API.
*SPI configuration: 
*	Full duplex 
*	STM32F4xx is master, Arduino is slave
*	8-bits data frame
*	Hardware slave management
*	SCLK speed = 2MHz
*
*@author Tran Thanh Nhan
*@date 01/08/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*User_button PA0
*SPI3_MOSI PC12
*SPI3_MISO PC11
*SPI3_SCLK PC10
*SPI3_NSS PA15
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_spi.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include "string.h"

void delay (void)
{
	for (int i = 0;i < 500000;i++){}
}

int main (void)
{
	RCC_MCO2_config(RCC_MCO2_SYSCLK,RCC_MCO_DIV5);
	RCC_set_SYSCLK_PLL_84_MHz();
	
	/*initilize green led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*Initialize user button on PA0*/
	button_init(GPIOA,GPIO_PIN_NO_0,GPIO_PDR);
	
	GPIO_init_direct(GPIOA,GPIO_PIN_NO_15,GPIO_MODE_ALTFN,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,6);
	
	/*Initilize SPI2 on PB15:PB12*/
	SPI_general_init(SPI3,SPI_pins_pack_2,SPI_MODE_MASTER,SPI_BUS_FULL_DUPLEX,SPI_DATA_8BITS,SPI_CLK_PHASE_1ST_E,SPI_CLK_POL_LIDLE,SPI_SSM_EN,SPI_CLK_SPEED_DIV2);
	SPI_SSI_ctr(SPI3,ENABLE);
		
	SPI_periph_ctr(SPI3,ENABLE);
	
	char *MsgPtr = "This is master STM32F4 sending message through SPI";
	uint8_t MsgLength = strlen(MsgPtr);
	
	while(1){
		if(button_read(GPIOA,GPIO_PIN_NO_0)){
			delay();
			led_on(GPIOD,GPIO_PIN_NO_12);
			
			SPI_send_data(SPI3,&MsgLength,1);
			SPI_send_data(SPI3,(uint8_t*)MsgPtr,MsgLength);
			
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}