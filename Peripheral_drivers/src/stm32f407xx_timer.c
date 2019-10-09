/**
*@file stm32f407xx_dac.c
*@brief provide APIs for interfacing with basic timer on stm32f407xx MCUs.
*
*This source file provide APIs for interfacing with basic timer (timer 6 and timer 7)on stm32f407xx MCUs.
*@note: only use this driver for timer 6 and timer 7.
*
*@author Tran Thanh Nhan
*@date 20/08/2019
*/

#include "../inc/stm32f407xx_timer.h"

/***********************************************************************
Timer clock enable/disable
***********************************************************************/
void TIM_CLK_ctr(TIM_TypeDef *TIMxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		
		if(TIMxPtr ==  TIM3){
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		}else if (TIMxPtr ==  TIM4){
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
		}else if(TIMxPtr ==  TIM6){
			RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
		}else if (TIMxPtr ==  TIM7){
			RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
		}
		
	}else if (enOrDis == DISABLE){
		
		if(TIMxPtr ==  TIM3){
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
		}else if (TIMxPtr ==  TIM4){
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN;
		}else if(TIMxPtr ==  TIM6){
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM6EN;
		}else if (TIMxPtr ==  TIM7){
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM7EN;
		}
	}	
}

/***********************************************************************
Timer start/stop 
***********************************************************************/
void TIM_ctr(TIM_TypeDef *TIMxPtr, uint8_t startOrStop)
{
	if(startOrStop == START){
		TIMxPtr->CR1 |= TIM_CR1_CEN;
	}else{
		TIMxPtr->CR1 &= ~TIM_CR1_CEN;
	}
}

/***********************************************************************
Initilize timer
***********************************************************************/
void TIM_init(TIM_Handle_t *TIMxHandlePtr)
{
	/*enable clock to timer*/
	TIM_CLK_ctr(TIMxHandlePtr->TIMxPtr,ENABLE);
	
	/*stop timer for initialization*/
	TIM_ctr(TIMxHandlePtr->TIMxPtr,STOP);
	
	/*only allow counter overflow to generate interrupt*/
	TIMxHandlePtr->TIMxPtr->CR1 |= TIM_CR1_URS;
	
	/*config reload value*/
	uint16_t reloadVal = TIMxHandlePtr->TIMxConfigPtr->reloadVal;
	TIMxHandlePtr->TIMxPtr->ARR = reloadVal;
	
	/*config prescaler*/
	uint16_t prescaler = TIMxHandlePtr->TIMxConfigPtr->prescaler;
	TIMxHandlePtr->TIMxPtr->PSC = prescaler;
	
	/*update reload value and prescaler immediately*/
	TIMxHandlePtr->TIMxPtr->EGR |= TIM_EGR_UG;
}

/***********************************************************************
Initialize timer directly using given parameter
***********************************************************************/
void TIM_init_direct(TIM_TypeDef *TIMxPtr,uint16_t reloadVal,uint16_t preScaler)
{
	TIM_Handle_t TIMxHandle;
	TIM_Config_t TIMxConfig;
	
	TIMxConfig.reloadVal = reloadVal;
	TIMxConfig.prescaler = preScaler;
	
	TIMxHandle.TIMxPtr = TIMxPtr;
	TIMxHandle.TIMxConfigPtr = &TIMxConfig;
	
	TIM_init(&TIMxHandle);
}

/***********************************************************************
Deinitialize timer
***********************************************************************/
void TIM_deinit(TIM_TypeDef *TIMxPtr)
{
	
	if(TIMxPtr ==  TIM3){
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
	}else if (TIMxPtr ==  TIM4){
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM4RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM4RST;
	}else if(TIMxPtr == TIM6){
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
		RCC->AHB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
	}else if(TIMxPtr == TIM7){
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM7RST;
		RCC->AHB1RSTR &= ~RCC_APB1RSTR_TIM7RST;
	}
}

/***********************************************************************
Reset timer counter
***********************************************************************/
void TIM_reset_counter(TIM_TypeDef *TIMxPtr)
{
	TIMxPtr->EGR |= TIM_EGR_UG;
}

/***********************************************************************
Set timer 's reload value
***********************************************************************/
void TIM_set_reload_val(TIM_TypeDef *TIMxPtr, uint16_t reloadVal)
{
	/*update reload value immediately*/
	TIMxPtr->ARR = reloadVal;
	TIMxPtr->EGR |= TIM_EGR_UG;
}

/***********************************************************************
Set timer 's prescaler
***********************************************************************/
void TIM_set_prescaler(TIM_TypeDef *TIMxPtr, uint16_t prescaler)
{
	/*update prescaler immediately*/
	TIMxPtr->PSC = prescaler;
	TIMxPtr->EGR |= TIM_EGR_UG;
}

/***********************************************************************
Enable or disable interrupt for update event of timer
***********************************************************************/
void TIM_interrupt_ctr(TIM_TypeDef *TIMxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		TIMxPtr->DIER |= TIM_DIER_UIE;
	}else{
		TIMxPtr->DIER &= ~TIM_DIER_UIE;
	}		
}

/***********************************************************************
Enable or disable timer 's interrupt vector in NVIC 
***********************************************************************/
void TIM_intrpt_vector_ctr (uint8_t IRQnumber, uint8_t enOrDis)
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

/***********************************************************************
Config priority for timer 's interrupt 
***********************************************************************/
void TIM_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	NVIC->IP[registerNo] &= ~(0xFF << (8*section));
	NVIC->IP[registerNo] |= (priority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}

/***********************************************************************
Timer interrupt handler
***********************************************************************/
void TIM_intrpt_handler (TIM_TypeDef *TIMxPtr)
{
	uint8_t check1 = TIMxPtr->DIER & TIM_DIER_UIE;
	uint8_t check2 = TIMxPtr->SR & TIM_SR_UIF;
	
	if(check1 & check2){
		TIMxPtr->SR &= ~TIM_SR_UIF;
	}
}

/***********************************************************************
Configure timer trigger output (TRGO) on update event 
***********************************************************************/
void TIM_update_event_TRGO (TIM_TypeDef *TIMxPtr)
{
	TIMxPtr->CR2 |= 0x02 << TIM_CR2_MMS_Pos;
}
