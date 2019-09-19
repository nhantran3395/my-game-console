/**
*@brief test SPI receive data function by sending command and receiving message between STM32F4 Discovery Board and Arduino using SPI
*
*STM32F4 will send command to Arduino, then receive message from Arduino according to the command sent. 
*Then, the message will be send to PC through UART2.
*Purpose is to confirm the working of SPI receive data function.
*SPI configuration: 
*	Full duplex 
*	STM32F4 Discovery Board is in master mode, Arduino is in slave mode
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
*User_button - PA0
*SPI3_MOSI - PC12
*SPI3_MISO - PC11
*SPI3_SCLK - PC10
*SPI3_NSS - PA15
*UART2 TX	- PA2
*UART2 RX	- PA3
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_spi.h"
#include "../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include <string.h>
#include <stdio.h>

UART_Handle_t *UARTxHandlePtr;

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
	
	/*Initilize SPI2 on PC12:PC10*/
	SPI_general_init(SPI3,SPI_pins_pack_2,SPI_MODE_MASTER,SPI_BUS_FULL_DUPLEX,SPI_DATA_8BITS,SPI_CLK_PHASE_1ST_E,SPI_CLK_POL_LIDLE,SPI_SSM_DIS,SPI_CLK_SPEED_DIV8);
	SPI_NSS_pin_ctr(SPI3,ENABLE);
	SPI_periph_ctr(SPI3,ENABLE);
	
	/*Initialize UART2 on PA2,PA3*/
	UARTxHandlePtr = UART_general_init(USART2,UART_pins_pack_1,UART_BDR_9600,UART_STB_1,UART_WRDLEN_8_DT_BITS,UART_TX_RX,UART_NO_PARCTRL,UART_NO_FLOWCTRL);
	
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
	
			SPI_send_data(SPI3,&sendLengthCmd,1);
			SPI_receive_data(SPI3,&dummyRead,1);
			
			SPI_send_data(SPI3,&dummyWrite,1);
			SPI_receive_data(SPI3,&SPI_msg_length,1);
			
			sprintf(msg,"SPI message length: %d \n\r",SPI_msg_length);
			UART_send(UARTxHandlePtr,(uint8_t*)msg,strlen(msg));
			
			SPI_send_data(SPI3,&sendMsgCmd,1);
			SPI_receive_data(SPI3,&dummyRead,1);
			
			for(uint8_t count = 0; count < SPI_msg_length; count++){
				SPI_send_data(SPI3,&dummyWrite,1);
				SPI_receive_data(SPI3,(uint8_t*)(SPI_msg+count),1);
			}
			
			sprintf(msg,"SPI message: %s \n\r",SPI_msg);
			UART_send(UARTxHandlePtr,(uint8_t*)msg,strlen(msg));
			
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}