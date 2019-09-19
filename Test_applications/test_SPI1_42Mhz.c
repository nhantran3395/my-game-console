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
*SPI1_MOSI PB5
*SPI1_MISO PB4
*SPI1_SCLK PB3
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
	
	/*initilize green led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*Initialize user button on PA0*/
	button_init(GPIOA,GPIO_PIN_NO_0,GPIO_PDR);
	
	GPIO_init_direct(GPIOA,GPIO_PIN_NO_15,GPIO_MODE_ALTFN,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,6);
	
	/*Initilize SPI2 on PB15:PB12*/
	SPI_general_init(SPI1,SPI_pins_pack_2,SPI_MODE_MASTER,SPI_BUS_FULL_DUPLEX,SPI_DATA_8BITS,SPI_CLK_PHASE_1ST_E,SPI_CLK_POL_LIDLE,SPI_SSM_EN,SPI_CLK_SPEED_DIV2);
	SPI_SSI_ctr(SPI1,ENABLE);
		
	SPI_periph_ctr(SPI1,ENABLE);
	
	char *MsgPtr = "This is master STM32F4 sending message through SPI";
	uint8_t MsgLength = strlen(MsgPtr);
	
	while(1){
		if(button_read(GPIOA,GPIO_PIN_NO_0)){
			delay();
			led_on(GPIOD,GPIO_PIN_NO_12);
			
			SPI_send_data(SPI1,&MsgLength,1);
			SPI_send_data(SPI1,(uint8_t*)MsgPtr,MsgLength);
			
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}