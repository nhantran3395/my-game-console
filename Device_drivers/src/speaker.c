/**
*@file speaker.c
*@brief provide functions for interfacing with speaker
*
*This implementation file provide functions for interfacing with speaker.
*Speaker is driven with STM32F4 Discovery board 's on-board DAC (user can select channel 1 or channel 2).
*Timer 7 is used for creating time interval between sound samples.
*
*@author Tran Thanh Nhan
*@date 21/08/2019
*/

#include "stm32f407xx.h"
#include "../inc/speaker.h"
#include "../inc/led.h"

extern DAC_Handle_t DACxHandle;
const uint16_t *soundPtrGlobal = NULL;
const uint16_t *soundEnd = NULL;

void speaker_init (uint8_t DAC_channel, uint16_t timerPrescaler, uint16_t timerReload)
{

	DAC_init_channel(DAC_channel);

	TIM_init_direct(SPEAKER_TIMER,timerPrescaler,timerReload);
	
	/*enable timer update event interrupt and enable interrupt request of timer in NVIC*/
	TIM_interrupt_ctr(SPEAKER_TIMER,ENABLE);

	TIM_intrpt_vector_ctr(SPEAKER_TIMER_IRQ_NUM,ENABLE);

}

void speaker_play_sound (const uint16_t *soundPtr, uint16_t size)
{
	soundPtrGlobal = soundPtr;
	soundEnd = soundPtrGlobal + size - 1;
	TIM_ctr(SPEAKER_TIMER,START);
}

void speaker_stop_sound (void)
{
	soundPtrGlobal = NULL;
	soundEnd = NULL;
	TIM_ctr(SPEAKER_TIMER,STOP);
}

#ifdef SPEAKER_USE_TIMER7
	void TIM7_IRQHandler (void)
	{
		TIM_intrpt_handler(TIM7);
		DAC_write(&DACxHandle,*(soundPtrGlobal++));
		if(soundPtrGlobal == soundEnd){
			speaker_stop_sound();
		}
	}
#endif

#ifdef SPEAKER_USE_TIMER6
	void TIM6_DAC_IRQHandler (void)
	{
		TIM_intrpt_handler(TIM6);
		DAC_write(&DACxHandle,*(soundPtrGlobal++));
		if(soundPtrGlobal == soundEnd){
			speaker_stop_sound();
		}
	}
#endif

#ifdef SPEAKER_USE_TIMER3
	void TIM3_IRQHandler (void)
	{
		TIM_intrpt_handler(TIM3);
		DAC_write(&DACxHandle,*(soundPtrGlobal++));
		if(soundPtrGlobal == soundEnd){
			speaker_stop_sound();
		}
	}
#endif

#ifdef SPEAKER_USE_TIMER4
	void TIM4_IRQHandler (void)
	{
		TIM_intrpt_handler(TIM4);
		DAC_write(&DACxHandle,*(soundPtrGlobal++));
		if(soundPtrGlobal == soundEnd){
			speaker_stop_sound();
		}
	}
#endif
