/**
*@brief test STM32F4xx UART API 
*@author Tran Thanh Nhan
*@date 08/15/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*UART_TX PA2
*UART_RX PA3
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include "string.h"
#include "stdlib.h"

void delay (void)
{
	for (int i = 0;i < 3200000;i++){
	}
}

UART_Handle_t UART2Handle;

void UART2_GPIO_pin_init (void)
{	
	GPIO_Pin_config_t GPIO_UART2_pin_config = {.mode = GPIO_MODE_ALTFN,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_OD,.puPdr = GPIO_PU,.altFunc = 7};
	GPIO_Handle_t GPIO_UART2_pin_handle;
	GPIO_UART2_pin_handle.GPIOxPtr = GPIOA;

//	/*CTS*/
//	GPIO_UART2_pin_config.pinNumber = GPIO_PIN_NO_0;
//	GPIO_UART2_pin_handle.GPIO_Pin_config = GPIO_UART2_pin_config;
//	GPIO_init(&GPIO_UART2_pin_handle);
//	
//	/*RTS*/
//	GPIO_UART2_pin_config.pinNumber = GPIO_PIN_NO_1;
//	GPIO_UART2_pin_handle.GPIO_Pin_config = GPIO_UART2_pin_config;
//	GPIO_init(&GPIO_UART2_pin_handle);
	
	/*TX*/
	GPIO_UART2_pin_config.pinNumber = GPIO_PIN_NO_2;
	GPIO_UART2_pin_handle.GPIO_Pin_config = GPIO_UART2_pin_config;
	GPIO_init(&GPIO_UART2_pin_handle);
	
	/*RX*/
	GPIO_UART2_pin_config.pinNumber = GPIO_PIN_NO_3;
	GPIO_UART2_pin_handle.GPIO_Pin_config = GPIO_UART2_pin_config;
	GPIO_init(&GPIO_UART2_pin_handle);	
}

void UART2_init (void)
{
	UART_Config_t UART2Config = {.baudRate = UART_BDR_9600, .stopBit = UART_STB_1, .wordLength = UART_WRDLEN_8_DT_BITS, .mode = UART_TX_RX,.parityCtrl = UART_NO_PARCTRL, .flowCtrl = UART_NO_FLOWCTRL};
	UART2Handle.UARTxPtr = USART2;
	UART2Handle.UARTxConfigPtr = &UART2Config; 
	UART_init(&UART2Handle);
}

int main (void)
{
	/*initilize green led on PD12, red led on PD14, blue led on PD15*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	led_init(GPIOD,GPIO_PIN_NO_14);
	led_init(GPIOD,GPIO_PIN_NO_15);
	
	/*initialize user button on PA0*/
	button_init(GPIOA,GPIO_PIN_NO_0,GPIO_NO_PUPDR);
	
	/*initilize UART2 on PA0:PA3*/
	UART2_GPIO_pin_init();
	UART2_init();
	
	char msg[]="Testing STM32F4 UART transmitter (busy wait method)\n \r";
	
	while(1){
		if(button_read(GPIOA,GPIO_PIN_NO_0)){
//			led_on(GPIOD,GPIO_PIN_NO_12);
			delay();
			UART_send(&UART2Handle,(uint8_t*)msg,strlen(msg));
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
}