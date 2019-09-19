/**
*@brief test STM32F4xx ADC driver APIs 
*
*@author Tran Thanh Nhan
*@date 08/26/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_adc.h"
#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include "../Miscellaneous/inc/data.h"
#include "../Miscellaneous/inc/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UART_Handle_t UART2Handle;
uint8_t outDataFlag = CLEAR;

//void ADC_GPIO_pin_init (void)
//{	
//	GPIO_Pin_config_t GPIO_ADC_pin_config = {.mode = GPIO_MODE_ANL,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr = GPIO_PDR};
//	GPIO_Handle_t GPIO_ADC_pin_handle;
//	GPIO_ADC_pin_handle.GPIOxPtr = GPIOA;
//	
//	GPIO_ADC_pin_config.pinNumber = GPIO_PIN_NO_7;
//	GPIO_ADC_pin_handle.GPIO_Pin_config = GPIO_ADC_pin_config;
//	GPIO_init(&GPIO_ADC_pin_handle);	
//}

//void ADC1init (void)
//{
//	ADC_Handle_t ADC1Handle;
//	extern ADC_Config_t ADCxConfig;
//	ADC1Handle.ADCxPtr = ADC1;
//	ADC1Handle.ADCxConfigPtr = &ADCxConfig; 
//	ADC_init(&ADC1Handle);
//}

void TIM6_init(void)
{
	/*select reload value and prescaler so that TIM6 period is 500ms*/
	TIM_Config_t TIM6Config = {.reloadVal = 999,.prescaler = 7999};
	TIM_Handle_t TIM6Handle = {TIM6,&TIM6Config};
	TIM_init(&TIM6Handle);
}

void UART2_GPIO_pin_init (void)
{	
	GPIO_Pin_config_t GPIO_UART2_pin_config = {.mode = GPIO_MODE_ALTFN,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_OD,.puPdr = GPIO_PU,.altFunc = 7};
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

int main (void){
	/*initilize green led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*initialize ADC2 channel 0 on PA0*/
//	ADC_GPIO_pin_init();
//	ADC1init();
	ADC_init_channel(ADC1,ADC_CHANNEL_7);
	
	/*initialize TIM6, enable interrupt on TIM6 's update event, enable TIM6 interrupt vector in NVIC*/
	TIM6_init();
	TIM_interrupt_ctr(TIM6,ENABLE);
	TIM_intrpt_vector_ctr(IRQ_TIM6_DAC,ENABLE);
	
	/*initilize UART2 on PA0:PA3*/
	UART2_GPIO_pin_init();
	UART2_init();
	
	/*enable USART2 interrupt vector in NVIC*/
	UART_intrpt_vector_ctrl(IRQ_USART2,ENABLE);
	
	/*start TIM6*/
	TIM_ctr(TIM6,START);

	char str[10];
	
	while(1){
		if (outDataFlag == SET){
			sprintf(str,"%d\n\r",ADC_read(ADC1,ADC_CHANNEL_7));
			UART_send_intrpt(&UART2Handle,(uint8_t*)&str,strlen(str));
			outDataFlag = CLEAR;
		}
	}
}

void TIM6_DAC_IRQHandler (void)
{
	TIM_intrpt_handler(TIM6);
	led_toggle(GPIOD,GPIO_PIN_NO_12);
	outDataFlag = SET;
}

void USART2_IRQHandler(void)
{
	UART_intrpt_handler(&UART2Handle);
}
