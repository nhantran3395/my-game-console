/**
*@brief test joystick driver library 's functions
*
*This read direction of joystick (up,down,left,right,etc.) every 0.5 s and print to PC serial terminal.
*Purpose is to confirm the working of joystick driver library 's functions.
*The output in PC serial terminal is compared with analog voltage in joystick x & y axis pins to determind correctness. 
*
*@author Tran Thanh Nhan
*@date 08/26/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*Joystick x axis (ADC1 channel 7)	- PA7
*Joystick y axis (ADC1 channel 6)	- PA6
*UART2 TX	- PA2
*UART2 RX	- PA3
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UART_Handle_t *UART2HandlePtr;
uint8_t ADCOutFlag = CLEAR;

int main (void){
	/*initilize joystick with x axis and y axis is ADC1 channel 7 & ADC1 channel 6*/
	joystick_init(ADC1,ADC_CHANNEL_5,ADC_CHANNEL_7);
		
	/*initilize UART2 on PA2:PA3 (pins pack 1)*/
	UART2HandlePtr = UART_general_init(USART2,UART_pins_pack_1,UART_BDR_9600,UART_STB_1,UART_WRDLEN_8_DT_BITS,UART_TX_RX,UART_NO_PARCTRL,UART_NO_FLOWCTRL);
	
	/*enable USART2 interrupt vector in NVIC*/
	UART_intrpt_vector_ctrl(IRQ_USART2,ENABLE);
	
	/*initialize TIM6, enable interrupt on TIM6 's update event, enable TIM6 interrupt vector in NVIC*/
	TIM_init_direct(TIM6,999,7999);
	TIM_interrupt_ctr(TIM6,ENABLE);
	TIM_intrpt_vector_ctr(IRQ_TIM6_DAC,ENABLE);
	
	/*start TIM6*/
	TIM_ctr(TIM6,START);

	char str[100];
	
	while(1){
		if (ADCOutFlag == SET){
			joystick_init(ADC1,ADC_CHANNEL_5,ADC_CHANNEL_7);
			uint8_t jsDir = joystick_read_direction(ADC1,ADC_CHANNEL_5,ADC_CHANNEL_7);
			
			if(jsDir == JS_DIR_LEFT_UP){
				sprintf(str,"Direction: left up X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_LEFT_DOWN){
				sprintf(str,"Direction: left down X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_LEFT){
				sprintf(str,"Direction: left X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_RIGHT_DOWN){
				sprintf(str,"Direction: right down X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_RIGHT_UP){
				sprintf(str,"Direction: right up X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_RIGHT){
				sprintf(str,"Direction: right X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_UP){
				sprintf(str,"Direction: up X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_DOWN){
				sprintf(str,"Direction: down X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}else if (jsDir == JS_DIR_CENTERED){
				sprintf(str,"Direction: centered X:%d Y:%d\n\r",ADC_read(ADC1,ADC_CHANNEL_5),ADC_read(ADC1,ADC_CHANNEL_7));
			}
			
			UART_send_intrpt(UART2HandlePtr,(uint8_t*)&str,strlen(str));
			ADCOutFlag = CLEAR;
		}
	}
}

void TIM6_DAC_IRQHandler (void)
{
	TIM_intrpt_handler(TIM6);
	ADCOutFlag = SET;
}

void USART2_IRQHandler(void)
{
	UART_intrpt_handler(UART2HandlePtr);
}
