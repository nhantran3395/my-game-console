/**
*@brief test SPI send message APIs by sending data from STM32F4xx to Arduino using SPI (interrupt base method)
*
*This program send  message from STM32F4 to Arduino through SPI2 peripheral whenever user button on STM32F4 board is press. Purpose is to test SPI send data interrupt based function.
*SPI configuration: 
*	Full duplex 
*	STM32F4xx is master, Arduino is slave
*	8-bits data frame
*	Hardware slave management
*	SCLK speed = 2MHz
*
*@author Tran Thanh Nhan
*@date 30/08/2019
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
#include "../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include <string.h>
#include <stdio.h>

SPI_Handle_t *SPI3HandlePtr;
UART_Handle_t *UART2HandlePtr;

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
	SPI3HandlePtr = SPI_general_init(SPI3,SPI_pins_pack_2,SPI_MODE_MASTER,SPI_BUS_FULL_DUPLEX,SPI_DATA_8BITS,SPI_CLK_PHASE_1ST_E,SPI_CLK_POL_LIDLE,SPI_SSM_DIS,SPI_CLK_SPEED_DIV8);
	SPI_NSS_pin_ctr(SPI3,ENABLE);
	SPI_periph_ctr(SPI3,ENABLE);
	SPI_intrpt_vector_ctrl(IRQ_SPI3,ENABLE);
	
	/*Initialize UART2 on PA2,PA3*/
	UART2HandlePtr = UART_general_init(USART2,UART_pins_pack_1,UART_BDR_9600,UART_STB_1,UART_WRDLEN_8_DT_BITS,UART_TX_RX,UART_NO_PARCTRL,UART_NO_FLOWCTRL);
	
	uint8_t sendLengthCmd = 0x50;
	uint8_t sendMsgCmd = 0x51;
	
	char msg[100] = {0};
	uint8_t SPI_msg_length = 0;
	char SPI_msg[100] = {0};;
	uint8_t dummyWrite = 0xFF;
	uint8_t dummyRead = 0xFF;
	
	while(1){
		if(button_read(GPIOA,GPIO_PIN_NO_0)){
			delay();
			led_on(GPIOD,GPIO_PIN_NO_12);
			
			while(SPI_send_data_intrpt(SPI3HandlePtr,&sendLengthCmd,1) != SPI_STATE_READY);
			while(SPI_receive_data_intrpt(SPI3HandlePtr,&dummyRead,1) != SPI_STATE_READY);
			
			while(SPI_send_data_intrpt(SPI3HandlePtr,&dummyWrite,1) != SPI_STATE_READY);
			while(SPI_receive_data_intrpt(SPI3HandlePtr,&SPI_msg_length,1) != SPI_STATE_READY);
			
			sprintf(msg,"SPI message length: %d \n\r",SPI_msg_length);
			UART_send(UART2HandlePtr,(uint8_t*)msg,strlen(msg));
			
			while(SPI_send_data_intrpt(SPI3HandlePtr,&sendMsgCmd,1)!= SPI_STATE_READY);
			while(SPI_receive_data_intrpt(SPI3HandlePtr,&dummyRead,1)!=SPI_STATE_READY);
			
			for(uint8_t count = 0; count < SPI_msg_length; count++){
				while(SPI_send_data_intrpt(SPI3HandlePtr,&dummyWrite,1)!=SPI_STATE_READY);
				while(SPI_receive_data_intrpt(SPI3HandlePtr,(uint8_t*)(SPI_msg+count),1)!=SPI_STATE_READY);
			}
			
			sprintf(msg,"SPI message: %s \n\r",SPI_msg);
			UART_send(UART2HandlePtr,(uint8_t*)msg,strlen(msg));
			
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}

void SPI3_IRQHandler (void)
{
	SPI_intrpt_handler(SPI3HandlePtr);
}