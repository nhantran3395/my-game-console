/**
*@brief test rcc driver library 's functions
*
*@author Tran Thanh Nhan
*@date 06/09/2019
*/

/*
*@PIN_MAPPING
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Device_drivers/inc/led.h"

void delay(volatile uint32_t delay) {
	for (; delay != 0; delay--); 
}

uint32_t sysClock = 0;
uint32_t APB2Clock = 0;

int main (void)
{

	led_init(GPIOD,GPIO_PIN_NO_12);

//	RCC_MCO2_config(RCC_MCO2_SYSCLK,RCC_MCO_DIV5);
//	RCC_MCO1_config(RCC_MCO1_PLL,RCC_MCO_DIV5);

	RCC_set_SYSCLK_PLL_84_MHz();
	sysClock = RCC_get_SYSCLK_value();
	APB2Clock = RCC_get_PCLK_value(APB2);
	
	TIM_init_direct(TIM6,20999,999);
	TIM_intrpt_vector_ctr(IRQ_TIM6_DAC,ENABLE);
	TIM_interrupt_ctr(TIM6,ENABLE);
	TIM_ctr(TIM6,START);

	while(1);
}

void TIM6_DAC_IRQHandler (void)
{
	TIM_intrpt_handler(TIM6);
	led_toggle(GPIOD,GPIO_PIN_NO_12);
}
