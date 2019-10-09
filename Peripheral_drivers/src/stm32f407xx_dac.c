/**
*@file stm32f407xx_dac.c
*@brief provide APIs for interfacing with DAC on stm32f407xx MCUs.
*
*This source file provide APIs for interfacing with DAC on stm32f407xx MCUs.
*
*@author Tran Thanh Nhan
*@date 19/08/2019
*/

#include "../inc/stm32f407xx_dac.h"

DAC_Handle_t DACxHandle;
DAC_Config_t DACxConfig;

/***********************************************************************
DAC clock enable/disable
***********************************************************************/
void DAC_CLK_ctr(DAC_TypeDef *DACxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	}else{
		RCC->APB1ENR &= ~RCC_APB1ENR_DACEN;
	}	
}

/***********************************************************************
DAC peripheral enable/disable
***********************************************************************/
void DAC_periph_ctr(DAC_Handle_t *DACxHandlePtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		if(DACxHandlePtr->DACxConfigPtr->channel == DAC_CHANNEL_1){
			DACxHandlePtr->DACxPtr->CR |= DAC_CR_EN1;
		}else if(DACxHandlePtr->DACxConfigPtr->channel == DAC_CHANNEL_2){
			DACxHandlePtr->DACxPtr->CR |= DAC_CR_EN2;
		}
	}else{
		if(DACxHandlePtr->DACxConfigPtr->channel == DAC_CHANNEL_1){
			DACxHandlePtr->DACxPtr->CR &= ~DAC_CR_EN1;
		}else if(DACxHandlePtr->DACxConfigPtr->channel == DAC_CHANNEL_2){
			DACxHandlePtr->DACxPtr->CR &= ~DAC_CR_EN2;
		}
	}
}

/***********************************************************************
Initialize DAC
***********************************************************************/
void DAC_init(DAC_Handle_t *DACxHandlePtr)
{
	/*enable clock for DAC peripheral*/
	DAC_CLK_ctr(DACxHandlePtr->DACxPtr,ENABLE);
	
	/*disable DAC peripheral for initialization*/
	DAC_periph_ctr(DACxHandlePtr,DISABLE);
	
	/*DAC channel trigger configuration*/
	uint8_t option = DACxHandlePtr->DACxConfigPtr->triggerEV;
	uint8_t channel =	DACxHandlePtr->DACxConfigPtr->channel;
	
	if(option != DAC_NO_TRIGGER_EV){
		if(channel == DAC_CHANNEL_1){
			DACxHandlePtr->DACxPtr->CR |= DAC_CR_TEN1;
			DACxHandlePtr->DACxPtr->CR &= ~DAC_CR_TSEL1;
			DACxHandlePtr->DACxPtr->CR |= option << DAC_CR_TSEL1_Pos;
		}else if(channel == DAC_CHANNEL_2){
			DACxHandlePtr->DACxPtr->CR |= DAC_CR_TEN2;
			DACxHandlePtr->DACxPtr->CR &= ~DAC_CR_TSEL2;
			DACxHandlePtr->DACxPtr->CR |= option << DAC_CR_TSEL2_Pos;
		}
	}
	
	/*DAC output buffer enable/disable*/
	option = DACxHandlePtr->DACxConfigPtr->outputBuffer;
	
	if(channel == DAC_CHANNEL_1){
		DACxHandlePtr->DACxPtr->CR &= ~DAC_CR_BOFF1;
		DACxHandlePtr->DACxPtr->CR |= option<<DAC_CR_BOFF1_Pos;
	}else if(channel == DAC_CHANNEL_2){
		DACxHandlePtr->DACxPtr->CR &= ~DAC_CR_BOFF2;
		DACxHandlePtr->DACxPtr->CR |= option<<DAC_CR_BOFF2_Pos;
	}
	
	/*enable DAC peripheral*/
	DAC_periph_ctr(DACxHandlePtr,ENABLE);
}

/***********************************************************************
Initialize DAC channel
***********************************************************************/
void DAC_init_channel(uint8_t channel)
{
	if(channel == DAC_CHANNEL_1){
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_4,GPIO_MODE_ANL,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,0);
	}else if(channel == DAC_CHANNEL_2){
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_5,GPIO_MODE_ANL,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,0);
	}

	DACxConfig.resolution 	= DAC_RES_12_bits;
	DACxConfig.alignment  	= DAC_ALIGNMENT_RIGHT;
	DACxConfig.triggerEV = DAC_NO_TRIGGER_EV;
	DACxConfig.outputBuffer = DAC_OBUFFER_EN;

	if(channel == DAC_CHANNEL_1){
		DACxConfig.channel = DAC_CHANNEL_1;
	}else if (channel == DAC_CHANNEL_2){
		DACxConfig.channel = DAC_CHANNEL_2;
	}

	DACxHandle.DACxConfigPtr = &DACxConfig;
	DACxHandle.DACxPtr = DAC;
	DAC_init(&DACxHandle);
}

/***********************************************************************
Deinitialize DAC
***********************************************************************/
void DAC_deinit(DAC_TypeDef *DACxPtr)
{
		RCC->APB1RSTR |= RCC_APB1RSTR_DACRST;
		RCC->APB1RSTR &= ~(RCC_APB1RSTR_DACRST);
}

/***********************************************************************
Write digital value to DAC 
***********************************************************************/
void DAC_write(DAC_Handle_t *DACxHandlePtr, uint16_t digiVal)
{
	uint8_t resolution = DACxHandlePtr->DACxConfigPtr->resolution;
	uint8_t channel = DACxHandlePtr->DACxConfigPtr->channel;

	if(channel == DAC_CHANNEL_1){
		/*case 8 bits DAC resolution*/
		if(resolution == DAC_RES_8_bits){
			DACxHandlePtr->DACxPtr->DHR8R1 = (uint8_t)(digiVal & 0xFF);
		/*case 12 bits DAC resolution*/
		}else{
			uint8_t alignment = DACxHandlePtr->DACxConfigPtr->alignment;
			if(alignment == DAC_ALIGNMENT_LEFT){
				DACxHandlePtr->DACxPtr->DHR12L1 = (digiVal & 0xFFF)<<DAC_DHR12L1_DACC1DHR_Pos;
			}else{
				DACxHandlePtr->DACxPtr->DHR12R1 = digiVal & 0xFFF;
			}
		}
	}else if(channel == DAC_CHANNEL_2){
		/*case 8 bits DAC resolution*/
		if(resolution == DAC_RES_8_bits){
			DACxHandlePtr->DACxPtr->DHR8R2 = (uint8_t)(digiVal & 0xFF);
		/*case 12 bits DAC resolution*/
		}else{
			uint8_t alignment = DACxHandlePtr->DACxConfigPtr->alignment;
			if(alignment == DAC_ALIGNMENT_LEFT){
				DACxHandlePtr->DACxPtr->DHR12L2 = (digiVal & 0xFFF)<<DAC_DHR12L2_DACC2DHR_Pos;
			}else{
				DACxHandlePtr->DACxPtr->DHR12R2 = digiVal & 0xFFF;
			}
		}
	}
}
