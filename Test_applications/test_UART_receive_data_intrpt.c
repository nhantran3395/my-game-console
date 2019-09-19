/**
*@brief test UART receive data APIs by echoing character between PC and STM32F4 discovery board
*
*When user type any character into serial terminal in PC, STM32F4 discovery board receive the character and send it back to PC.
*The character will be printed in serial terminal.
*During character reception, when no new character is being sent  from PC, green led on discovery board will blink.
*This show the difference between busy-wait based method and interrupt based method.
*With busy wait, when user have not yet type in any character, the board can only wait until user type in a character and can not perform any task during that.
*With interrupt, the board can perform other task ( like blinking led ) while waiting for user to type in a character.
*Purpose of the program is to confirm the working of UART receive data API (interrupt based method).
*
*@author Tran Thanh Nhan
*@date 08/23/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*UART2_TX PA2
*UART2_RX PA3
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include "../Peripheral_drivers/inc/stm32f407xx_common_macro.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include "string.h"
#include "stdlib.h"

void delay (void)
{
	for (int i = 0;i < 100000;i++){
	}
}

UART_Handle_t UART2Handle;
uint8_t rxCmplt = CLEAR;

void UART2_GPIO_pin_init (void)
{	
	GPIO_Pin_config_t GPIO_UART2_pin_config = {.mode = GPIO_MODE_ALTFN,.outType = GPIO_OUTPUT_TYPE_OD,.puPdr = GPIO_PU,.altFunc = 7};
	GPIO_Handle_t GPIO_UART2_pin_handle;
	GPIO_UART2_pin_handle.GPIOxPtr = GPIOA;

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
	static UART_Config_t UART2Config = {.baudRate = UART_BDR_9600, .stopBit = UART_STB_1, .wordLength = UART_WRDLEN_8_DT_BITS, .mode = UART_TX_RX,.parityCtrl = UART_NO_PARCTRL, .flowCtrl = UART_NO_FLOWCTRL};
	UART2Handle.UARTxPtr = USART2;
	UART2Handle.UARTxConfigPtr = &UART2Config; 
	UART_init(&UART2Handle);
}

int main (void)
{
	/*initialize green led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*initilize UART2 on PA2:PA3*/
	UART2_GPIO_pin_init();
	UART2_init();

	/*enable USART2 interrupt vector in NVIC*/
	UART_intrpt_vector_ctrl(IRQ_USART2,ENABLE);
	
	uint8_t rxChar = 0;
	
	while(1){
		while(UART_receive_intrpt(&UART2Handle,&rxChar,1) == UART_STATE_RX_BUSY);
		while(rxCmplt != SET){
			led_toggle(GPIOD,GPIO_PIN_NO_12);
			delay();
		}
		UART_send(&UART2Handle,&rxChar,1);
		rxCmplt = CLEAR;
	}
}

void USART2_IRQHandler(void)
{
	UART_intrpt_handler(&UART2Handle);
}

void UART_application_event_callback (UART_Handle_t *UARTxHandlePtr,uint8_t event) 
{
	if(event == UART_EV_RX_COMPLETE){
		rxCmplt = SET;
	}
}