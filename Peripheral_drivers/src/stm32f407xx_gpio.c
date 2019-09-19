/**
*@file stm32f407xx_gpio.c
*@brief provide functions for interfacing with GPIO pin
*
*This source file provide functions for interfacing with GPIO pins on stm32f407xx MCUs.
*
*@author Tran Thanh Nhan
*@date 23/07/2019
*/

#include "../inc/stm32f407xx_gpio.h"  

void GPIO_CLK_ctr (GPIO_TypeDef *GPIOxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
				if(GPIOxPtr == GPIOA){
					RCC->AHB1ENR |= (1<<0);
				}else if(GPIOxPtr == GPIOB){
					RCC->AHB1ENR |= (1<<1);
				}else if(GPIOxPtr == GPIOC){
					RCC->AHB1ENR |= (1<<2);
				}else if(GPIOxPtr == GPIOD){
					RCC->AHB1ENR |= (1<<3);
				}else if(GPIOxPtr == GPIOE){
					RCC->AHB1ENR |= (1<<4);
				}else if(GPIOxPtr == GPIOF){
					RCC->AHB1ENR |= (1<<5);
				}else if(GPIOxPtr == GPIOG){
					RCC->AHB1ENR |= (1<<6);
				}else if(GPIOxPtr == GPIOH){
					RCC->AHB1ENR |= (1<<7);
				}else if(GPIOxPtr == GPIOI){
					RCC->AHB1ENR |= (1<<8);
				}
	}else{
				if(GPIOxPtr == GPIOA){
					RCC->AHB1ENR &= ~(1<<0);
				}else if(GPIOxPtr == GPIOB){
					RCC->AHB1ENR &= ~(1<<1);
				}else if(GPIOxPtr == GPIOC){
					RCC->AHB1ENR &= ~(1<<2);
				}else if(GPIOxPtr == GPIOD){
					RCC->AHB1ENR &= ~(1<<3);
				}else if(GPIOxPtr == GPIOE){
					RCC->AHB1ENR &= ~(1<<4);
				}else if(GPIOxPtr == GPIOF){
					RCC->AHB1ENR &= ~(1<<5);
				}else if(GPIOxPtr == GPIOG){
					RCC->AHB1ENR &= ~(1<<6);
				}else if(GPIOxPtr == GPIOH){
					RCC->AHB1ENR &= ~(1<<7);
				}else if(GPIOxPtr == GPIOI){
					RCC->AHB1ENR &= ~(1<<8);
				}
	}
}

void GPIO_init (GPIO_Handle_t *GPIOxHandlePtr)
{
	GPIO_CLK_ctr(GPIOxHandlePtr->GPIOxPtr,ENABLE);
	
	/*configure GPIO pin mode*/
	uint8_t valueSet = GPIOxHandlePtr->GPIO_Pin_config.mode;
	uint8_t bitPos = GPIOxHandlePtr->GPIO_Pin_config.pinNumber;
	GPIOxHandlePtr->GPIOxPtr->MODER &= ~(0x03<<2*bitPos);
	
	if(valueSet <= GPIO_MODE_ANL){		
		GPIOxHandlePtr->GPIOxPtr->MODER |= valueSet<<(2*bitPos);
	}else{
		if (valueSet == GPIO_MODE_INTRPT_FE){
				EXTI->FTSR |= (1<<bitPos);
				EXTI->RTSR &= ~(1<<bitPos);
		}else if (valueSet == GPIO_MODE_INTRPT_RE){
				EXTI->RTSR |= (1<<bitPos);
				EXTI->FTSR &= ~(1<<bitPos);
		}else if (valueSet == GPIO_MODE_INTRPT_RFE){
				EXTI->RTSR |= 1<<bitPos;
				EXTI->FTSR |= 1<<bitPos;
		}
	
		/*GPIO port selection in EXTICR*/
		uint8_t registerNo = GPIOxHandlePtr->GPIO_Pin_config.pinNumber/4;
		bitPos = GPIOxHandlePtr->GPIO_Pin_config.pinNumber%4;
		if(GPIOxHandlePtr->GPIOxPtr == GPIOA){
			valueSet = 0x00;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOB){
			valueSet = 0x01;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOC){
			valueSet = 0x02;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOD){
			valueSet = 0x03;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOE){
			valueSet = 0x04;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOF){
			valueSet = 0x05;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOG){
			valueSet = 0x06;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOH){
			valueSet = 0x07;
		}else if(GPIOxHandlePtr->GPIOxPtr == GPIOI){
			valueSet = 0x08;
		}
		RCC->APB2ENR |= (1<<14);
		SYSCFG->EXTICR[registerNo] &= ~(0x0F<<4*bitPos);
		SYSCFG->EXTICR[registerNo] |= (valueSet<<4*bitPos);
	
		bitPos = GPIOxHandlePtr->GPIO_Pin_config.pinNumber;
		EXTI->IMR |= 1<<bitPos;
	}
	
	/*configure GPIO output speed*/
	valueSet = GPIOxHandlePtr->GPIO_Pin_config.speed;
	bitPos = GPIOxHandlePtr->GPIO_Pin_config.pinNumber;
	GPIOxHandlePtr->GPIOxPtr->OSPEEDR &= ~(0x03<<2*bitPos);
	GPIOxHandlePtr->GPIOxPtr->OSPEEDR |= valueSet<<(2*bitPos);
	
	/*configure GPIO output type*/
	valueSet = GPIOxHandlePtr->GPIO_Pin_config.outType;
	bitPos = GPIOxHandlePtr->GPIO_Pin_config.pinNumber;
	GPIOxHandlePtr->GPIOxPtr->OTYPER &= ~(0x01<<bitPos);
	GPIOxHandlePtr->GPIOxPtr->OTYPER |= valueSet<<(bitPos);
	
	/*configure GPIO pin pull up or pull down*/
	valueSet = GPIOxHandlePtr->GPIO_Pin_config.puPdr;
	bitPos = GPIOxHandlePtr->GPIO_Pin_config.pinNumber;
	GPIOxHandlePtr->GPIOxPtr->PUPDR &= ~(0x03<<2*bitPos);
	GPIOxHandlePtr->GPIOxPtr->PUPDR |= valueSet<<(2*bitPos);

	/*configure GPIO pin alternate function*/
	valueSet = GPIOxHandlePtr->GPIO_Pin_config.altFunc;
	bitPos = (GPIOxHandlePtr->GPIO_Pin_config.pinNumber)%8;
	uint8_t registerNo = (GPIOxHandlePtr->GPIO_Pin_config.pinNumber)/8;
	GPIOxHandlePtr->GPIOxPtr->AFR[registerNo] &= ~(0x0F<<4*bitPos);
	GPIOxHandlePtr->GPIOxPtr->AFR[registerNo] |= valueSet<<(4*bitPos);
}

void GPIO_init_direct (GPIO_TypeDef *GPIOxPtr,uint8_t pinNumber,uint8_t mode,uint8_t speed, uint8_t outType, uint8_t puPdr, uint8_t altFunc)
{
	GPIO_Handle_t GPIOxHandle;
	GPIO_Pin_config_t GPIOxConfig;
	
	GPIOxConfig.pinNumber = pinNumber;
	GPIOxConfig.mode = mode;
	GPIOxConfig.speed = speed;
	GPIOxConfig.outType = outType;
	GPIOxConfig.puPdr = puPdr;
	GPIOxConfig.altFunc = altFunc;
	
	GPIOxHandle.GPIOxPtr = GPIOxPtr;
	GPIOxHandle.GPIO_Pin_config = GPIOxConfig;
	GPIO_init(&GPIOxHandle);
}

void GPIO_deinit (GPIO_TypeDef *GPIOxPtr)
{
	if(GPIOxPtr == GPIOA){
		RCC->AHB1RSTR |= (1<<0);
		RCC->AHB1RSTR &= ~(1<<0);
	}else if(GPIOxPtr == GPIOB){
		RCC->AHB1RSTR |= (1<<1);
		RCC->AHB1RSTR &= ~(1<<1);
	}else if(GPIOxPtr == GPIOC){
		RCC->AHB1RSTR |= (1<<2);
		RCC->AHB1RSTR &= ~(1<<2);
	}else if(GPIOxPtr == GPIOD){
		RCC->AHB1RSTR |= (1<<3);
		RCC->AHB1RSTR &= ~(1<<3);
	}else if(GPIOxPtr == GPIOE){
		RCC->AHB1RSTR |= (1<<4);
		RCC->AHB1RSTR &= ~(1<<4);
	}else if(GPIOxPtr == GPIOF){
		RCC->AHB1RSTR |= (1<<5);
		RCC->AHB1RSTR &= ~(1<<5);
	}else if(GPIOxPtr == GPIOG){
		RCC->AHB1RSTR |= (1<<6);
		RCC->AHB1RSTR &= ~(1<<6);
	}else if(GPIOxPtr == GPIOH){
		RCC->AHB1RSTR |= (1<<7);
		RCC->AHB1RSTR &= ~(1<<7);
	}else if(GPIOxPtr == GPIOI){
		RCC->AHB1RSTR |= (1<<8);
		RCC->AHB1RSTR &= ~(1<<8);
	}
}

uint8_t GPIO_read_pin (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber)
{
	return (uint8_t)(GPIOxPtr->IDR >> pinNumber) & 0x01;
}

uint16_t GPIO_read_port (GPIO_TypeDef *GPIOxPtr)
{
	return (uint16_t)GPIOxPtr->IDR;
}

void GPIO_write_pin (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber, uint8_t setOrClear)
{
	if(setOrClear == SET){
		GPIOxPtr->ODR |= 1<<pinNumber;
	}else{
		GPIOxPtr->ODR &= ~(1<<pinNumber);
	}
}

void GPIO_write_port (GPIO_TypeDef *GPIOxPtr,uint16_t value)
{
		GPIOxPtr->ODR = value;
}

void GPIO_toggle_pin (GPIO_TypeDef *GPIOxPtr, uint8_t pinNumber)
{
		GPIOxPtr->ODR ^= 1<<pinNumber;
}

void GPIO_Intrpt_priority_config (uint8_t IRQnumber, uint8_t IRQpriority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	*(NVIC_IPR_BASE_ADDR + 4*registerNo) &= ~(0xFF << (8*section));
	*(NVIC_IPR_BASE_ADDR + 4*registerNo) |= (IRQpriority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}

void GPIO_Intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		if(IRQnumber <= 31){
			NVIC->ISER[0] |= (1<<IRQnumber);
		}
		else if(IRQnumber > 31 && IRQnumber <= 63){
			NVIC->ISER[1] |= (1<<(IRQnumber%32));
		}
		else if(IRQnumber > 63 && IRQnumber <= 95){
			NVIC->ISER[2] |= (1<<(IRQnumber%64));
		}
	}else{
		if(IRQnumber <= 31){
			NVIC->ICER[0] |= (1<<IRQnumber);
		}
		else if(IRQnumber > 31 && IRQnumber <= 63){
			NVIC->ICER[1] |= (1<<(IRQnumber%32));
		}
		else if(IRQnumber > 63 && IRQnumber <= 95){
			NVIC->ICER[2] |= (1<<(IRQnumber%64));
		}
	}
}

void GPIO_Intrpt_handler (uint8_t pinNumber)
{
	if(EXTI->PR & (1<<pinNumber)){
		EXTI->PR |= (1<<pinNumber);
	}
}
 