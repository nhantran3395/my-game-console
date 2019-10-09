/**
*@file stm32f407xx_common_macro.h
*@brief provide common macro
*
*This header file provide common macros used in self developed stm32f407xx MCU 's driver libraries.
*
*@author Tran Thanh Nhan
*@date 29/07/2019
*/

#ifndef STM32F407XX_COMMON_MACRO_H
#define STM32F407XX_COMMON_MACRO_H

#include <stdint.h>

#define ENABLE 1
#define DISABLE 0
#define SET 1
#define CLEAR 0
#define START 1
#define STOP 0
#define READ 1
#define WRITE 0
#define ON 1
#define OFF 0
#define TRUE	1
#define	FALSE	0

/*
*ARM cortex M4 Processor NVIC IPR register base address
*/
#define NVIC_IPR_BASE_ADDR 	((volatile uint32_t*)0xE000E400)
#define NUM_OF_IPR_BIT_IMPLEMENTED 4

/*
*IRQ number
*/
#define IRQ_EXTI0 6
#define IRQ_EXTI1 7
#define IRQ_EXTI2 8
#define IRQ_EXTI3 9
#define IRQ_EXTI4 10
#define IRQ_EXTI9_5 23
#define IRQ_EXTI15_10 40
#define IRQ_SPI1	35
#define IRQ_SPI2	36
#define IRQ_SPI3	51
#define IRQ_I2C1_EV	31
#define IRQ_I2C1_ER	32
#define IRQ_I2C2_EV	33
#define IRQ_I2C2_ER	34
#define IRQ_I2C3_EV	72
#define IRQ_I2C3_ER	73
#define IRQ_USART1 37
#define IRQ_USART2 38
#define IRQ_USART3 39
#define IRQ_USART4 52
#define IRQ_USART5 53
#define IRQ_USART6 71
#define IRQ_TIM3	29
#define IRQ_TIM4 	30
#define IRQ_TIM6_DAC 54
#define IRQ_TIM7 55
#define IRQ_HASH_RNG 80

#endif 
