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

#include "stm32f407xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Device_drivers/inc/led.h"
#include "string.h"
#include "stdlib.h"

int main (void){
	RCC_set_SYSCLK_PLL_84_MHz();
	
	/*initilize green led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*initialize TIM6, enable interrupt on TIM6 's update event, enable TIM6 interrupt vector in NVIC*/
	TIM_init_direct(TIM3,3749,999);
	TIM_interrupt_ctr(TIM3,ENABLE);
	TIM_intrpt_vector_ctr(IRQ_TIM3,ENABLE);
	
	/*start TIM6*/
	TIM_ctr(TIM3,START);
	
	while(1);
}

void TIM3_IRQHandler (void)
{
	TIM_intrpt_handler(TIM3);
	led_toggle(GPIOD,GPIO_PIN_NO_12);
}
