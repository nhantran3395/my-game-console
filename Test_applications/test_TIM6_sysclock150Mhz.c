/**
*@brief test driver APIs of STM32F407xx timer 
*
*This set PD12 pin to high for 500ms (t_on), then to low for 500ms (t_off) using TIM6 interrupt. 
*Logic analyzer is used to monitor PD12 state and to confirm whether t_on = t_off = 500ms. 
*Purpose is to test timer driver APIs. 
*
*@author Tran Thanh Nhan
*@date 08/20/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*Green_led PD12
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Device_drivers/inc/led.h"
#include "string.h"
#include "stdlib.h"

void TIM6_init(void)
{
	/*select reload value and prescaler so that TIM6 period is 500ms*/
	TIM_Config_t TIM6Config = {.reloadVal = 1874,.prescaler = 999};
	TIM_Handle_t TIM6Handle = {TIM6,&TIM6Config};
	TIM_init(&TIM6Handle);
}

int main (void){
	RCC_set_SYSCLK_PLL_84_MHz();
	
	/*initilize green led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*initialize TIM6, enable interrupt on TIM6 's update event, enable TIM6 interrupt vector in NVIC*/
	TIM6_init();
	TIM_interrupt_ctr(TIM6,ENABLE);
	TIM_intrpt_vector_ctr(IRQ_TIM6_DAC,ENABLE);
	
	/*start TIM6*/
	TIM_ctr(TIM6,START);
	
	while(1);
}

void TIM6_DAC_IRQHandler (void)
{
	TIM_intrpt_handler(TIM6);
	led_toggle(GPIOD,GPIO_PIN_NO_12);
}