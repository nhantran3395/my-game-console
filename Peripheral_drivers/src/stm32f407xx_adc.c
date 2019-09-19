/**
*@file stm32f407xx_adc.c
*@brief provide functions for interfacing with ADCs on stm32f407xx MCUs.
*
*This implementation file provide functions for interfacing with ADCs on stm32f407xx MCUs.
*
*@note This library only support the following configurations and features:
*	number of conversion: 1
*	resolution: 12 bits
*	conversion mode: single
*	read value using busy-wait method
*
*@author Tran Thanh Nhan
*@date 26/08/2019
*/

#include "../inc/stm32f407xx_adc.h"

void ADC_channel_0_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_1_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_2_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_3_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_4_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_5_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_6_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_7_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_8_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_9_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_10_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_11_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_12_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_13_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_14_init(ADC_TypeDef *ADCxPtr);
void ADC_channel_15_init(ADC_TypeDef *ADCxPtr);
void ADC_configure_channel (ADC_TypeDef *ADCxPtr,uint8_t channel);

/***********************************************************************
ADC clock enable/disable
***********************************************************************/
void ADC_CLK_ctr(ADC_TypeDef *ADCxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		if(ADCxPtr == ADC1){
			RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		}else if(ADCxPtr == ADC2){
			RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;		
		}else if(ADCxPtr == ADC3){
			RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;		
		}
	}else{
		if(ADCxPtr == ADC1){
			RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
		}else if(ADCxPtr == ADC2){
			RCC->APB2ENR &= ~RCC_APB2ENR_ADC2EN;		
		}else if(ADCxPtr == ADC3){
			RCC->APB2ENR &= ~RCC_APB2ENR_ADC3EN;		
		}
	}	
}

/***********************************************************************
ADC peripheral on/off
***********************************************************************/
void ADC_ctr(ADC_TypeDef *ADCxPtr, uint8_t onOrOff)
{
	if(onOrOff == ON){
		ADCxPtr->CR2 |= ADC_CR2_ADON;		
	}else if (onOrOff == OFF){
		ADCxPtr->CR2 &= ~ADC_CR2_ADON;
	}
}

/***********************************************************************
Initialize ADC
***********************************************************************/
void ADC_init(ADC_Handle_t *ADCxHandlePtr)
{
	/*enable clock for ADCx peripheral*/
	ADC_CLK_ctr(ADCxHandlePtr->ADCxPtr,ENABLE);
	
	/*turn off ADCx peripheral for initilization*/
	ADC_ctr(ADCxHandlePtr->ADCxPtr,DISABLE);
	
	/*set ADC resolution to 12 bits*/
	uint8_t option = ADCxHandlePtr->ADCxConfigPtr->resolution;
	ADCxHandlePtr->ADCxPtr->CR1 &= ~ADC_CR1_RES;
	ADCxHandlePtr->ADCxPtr->CR1 |= option<<ADC_CR1_RES_Pos;
	
	/*set conversion mode to single*/
	option = ADCxHandlePtr->ADCxConfigPtr->conversionMode;
	ADCxHandlePtr->ADCxPtr->CR2 &= ~ADC_CR2_CONT;
	ADCxHandlePtr->ADCxPtr->CR2 |= option<<ADC_CR2_CONT_Pos;
	
	/*set EOC as indicating end of each regular conversion*/
	ADCxHandlePtr->ADCxPtr->CR2 |= ADC_CR2_EOCS; 
	
	/*enable ADC peripheral*/
	ADC_ctr(ADCxHandlePtr->ADCxPtr,ENABLE);
}

/***********************************************************************
Initialize ADC channel 
***********************************************************************/
void ADC_init_channel(ADC_TypeDef *ADCxPtr,uint8_t channel)
{
	if(channel == ADC_CHANNEL_0){
		ADC_channel_0_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_1){
		ADC_channel_1_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_2){
		ADC_channel_2_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_3){
		ADC_channel_3_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_4){
		ADC_channel_4_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_5){
		ADC_channel_5_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_6){
		ADC_channel_6_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_7){
		ADC_channel_7_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_8){
		ADC_channel_8_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_9){
		ADC_channel_9_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_10){
		ADC_channel_10_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_11){
		ADC_channel_11_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_12){
		ADC_channel_12_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_13){
		ADC_channel_13_init(ADCxPtr);
	}else if(channel == ADC_CHANNEL_14){
		ADC_channel_14_init(ADCxPtr);		
	}else if(channel == ADC_CHANNEL_15){
		ADC_channel_15_init(ADCxPtr);
	}
		
	ADC_Handle_t ADCxHandle;
	static ADC_Config_t ADCxConfig = {.numOfConversion = 1, .resolution = ADC_RES_12_bits, .conversionMode = ADC_CVSMODE_SINGLE};
	
	ADCxHandle.ADCxConfigPtr = &ADCxConfig;
	ADCxHandle.ADCxPtr = ADCxPtr;	
	ADC_init(&ADCxHandle);
}

/***********************************************************************
Deinitialize ADC
***********************************************************************/
void ADC_deinit(void)
{
		RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;
		RCC->APB2RSTR &= ~(RCC_APB2RSTR_ADCRST);
}

/***********************************************************************
Write digital value to ADC 
***********************************************************************/
uint16_t ADC_read(ADC_TypeDef *ADCxPtr, uint8_t channel)
{
	uint16_t timeOut = 0xFFF;
	
	ADC_configure_channel(ADCxPtr,channel);
	
	/*execute software start*/
	ADCxPtr->CR2 |= ADC_CR2_SWSTART;
	
	/*wait until conversion end*/
	while(!(ADCxPtr->SR & ADC_SR_EOC))
	{
		timeOut--;
		if(!timeOut){
			return 0;
		}
	}
	return ADCxPtr->DR; 
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 0
***********************************************************************/
void ADC_channel_0_init(ADC_TypeDef *ADCxPtr)
{
	GPIO_init_direct(GPIOA,GPIO_PIN_NO_0,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 1
***********************************************************************/
void ADC_channel_1_init(ADC_TypeDef *ADCxPtr)
{
	GPIO_init_direct(GPIOA,GPIO_PIN_NO_1,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 2
***********************************************************************/
void ADC_channel_2_init(ADC_TypeDef *ADCxPtr)
{
	GPIO_init_direct(GPIOA,GPIO_PIN_NO_2,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 3
***********************************************************************/
void ADC_channel_3_init(ADC_TypeDef *ADCxPtr)
{
	GPIO_init_direct(GPIOA,GPIO_PIN_NO_3,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 4
***********************************************************************/
void ADC_channel_4_init(ADC_TypeDef *ADCxPtr)
{
	if(ADCxPtr == ADC1 || ADCxPtr == ADC2){
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_4,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}else if (ADCxPtr == ADC3){
		GPIO_init_direct(GPIOF,GPIO_PIN_NO_6,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 5
***********************************************************************/
void ADC_channel_5_init(ADC_TypeDef *ADCxPtr)
{
	if(ADCxPtr == ADC1 || ADCxPtr == ADC2){
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_5,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}else if (ADCxPtr == ADC3){
		GPIO_init_direct(GPIOF,GPIO_PIN_NO_7,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 6
***********************************************************************/
void ADC_channel_6_init(ADC_TypeDef *ADCxPtr)
{
	if(ADCxPtr == ADC1 || ADCxPtr == ADC2){
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_6,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}else if (ADCxPtr == ADC3){
		GPIO_init_direct(GPIOF,GPIO_PIN_NO_8,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 7
***********************************************************************/
void ADC_channel_7_init(ADC_TypeDef *ADCxPtr)
{
	if(ADCxPtr == ADC1 || ADCxPtr == ADC2){
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_7,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}else if (ADCxPtr == ADC3){
		GPIO_init_direct(GPIOF,GPIO_PIN_NO_9,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 8
***********************************************************************/
void ADC_channel_8_init(ADC_TypeDef *ADCxPtr)
{
	if(ADCxPtr == ADC1 || ADCxPtr == ADC2){
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_0,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}else if (ADCxPtr == ADC3){
		GPIO_init_direct(GPIOF,GPIO_PIN_NO_10,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 9
***********************************************************************/
void ADC_channel_9_init(ADC_TypeDef *ADCxPtr)
{
	if(ADCxPtr == ADC1 || ADCxPtr == ADC2){
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_1,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}else if (ADCxPtr == ADC3){
		GPIO_init_direct(GPIOF,GPIO_PIN_NO_3,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
	}
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 10
***********************************************************************/
void ADC_channel_10_init(ADC_TypeDef *ADCxPtr)
{
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_0,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 11
***********************************************************************/
void ADC_channel_11_init(ADC_TypeDef *ADCxPtr)
{
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_1,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 12
***********************************************************************/
void ADC_channel_12_init(ADC_TypeDef *ADCxPtr)
{
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_2,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 13
***********************************************************************/
void ADC_channel_13_init(ADC_TypeDef *ADCxPtr)
{
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_3,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 14
***********************************************************************/
void ADC_channel_14_init(ADC_TypeDef *ADCxPtr)
{
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_4,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: initialize GPIO pin corresponded to channel 15
***********************************************************************/
void ADC_channel_15_init(ADC_TypeDef *ADCxPtr)
{
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_5,GPIO_MODE_ANL,GPIO_OUTPUT_LOW_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_PU,0);
}

/***********************************************************************
Private function: configure regular channel
***********************************************************************/
void ADC_configure_channel (ADC_TypeDef *ADCxPtr,uint8_t channel){
	/*configure regular channel sequence length as 1*/
	ADCxPtr->SQR1 &=  ~ADC_SQR1_L_Pos;
	/*set channel as 1st conversion*/
	ADCxPtr->SQR3 &= ~ADC_SQR3_SQ1;
	ADCxPtr->SQR3 |= channel<<ADC_SQR3_SQ1_Pos;
}
