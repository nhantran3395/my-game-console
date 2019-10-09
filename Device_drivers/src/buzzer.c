/**
*@file buzzer.c
*@brief provide functions for interfacing with button
*
*This source file provide functions for interfacing with buzzer.
*Buzzer is driven with sinewave signal output from DAC output (user can select channel 1 or channel 2).
*Timer 6 is used to create time interval between samples of signal (Timer 6 's time base is changed to create signal with different frequencies) 
*Timer 7 is used to create duration for playing sound
*
*@author Tran Thanh Nhan
*@date 21/08/2019
*/

#include "../inc/buzzer.h"

uint16_t sineWaveTable[SAMPLE_NUM] = {2048,3251,3995,3996,3253,2051,847,101,98,839};
uint8_t count = 0;
uint8_t TIM7_flag = 0;

extern DAC_Handle_t DACxHandle;

void buzzer_init (uint8_t DAC_channel)
{
//	if (DAC_channel == DAC_CHANNEL_1){
//
//		/*configure GPIO PIN PA4*/
//		GPIO_Pin_config_t GPIO_DAC_CH1_pin_config = {.pinNumber = GPIO_PIN_NO_4,.mode = GPIO_MODE_ANL,};
//		GPIO_Handle_t GPIO_DAC_CH1_Handle = {GPIOA,GPIO_DAC_CH1_pin_config};
//		GPIO_init(&GPIO_DAC_CH1_Handle);
//
//		/*initilize DAC channel 1*/
//		static DAC_Config_t DAC_CH1_config = {.channel = DAC_CHANNEL_1,.resolution = DAC_RES_12_bits,.alignment = DAC_ALIGNMENT_RIGHT, .triggerEV = DAC_NO_TRIGGER_EV, .outputBuffer = DAC_OBUFFER_EN};
//		DACxHandle.DACxPtr = DAC1;
//		DACxHandle.DACxConfigPtr = &DAC_CH1_config;
//		DAC_init(&DACxHandle);
//
//	}else if(DAC_channel == DAC_CHANNEL_2){
//
//		/*configure GPIO PIN PA5*/
//		GPIO_Pin_config_t GPIO_DAC_CH2_pin_config = {.pinNumber = GPIO_PIN_NO_5,.mode = GPIO_MODE_ANL,};
//		GPIO_Handle_t GPIO_DAC_CH2_Handle = {GPIOA,GPIO_DAC_CH2_pin_config};
//		GPIO_init(&GPIO_DAC_CH2_Handle);
//
//		/*initilize DAC channel 2*/
//		static DAC_Config_t DAC_CH2_config = {.channel = DAC_CHANNEL_2,.resolution = DAC_RES_12_bits,.alignment = DAC_ALIGNMENT_RIGHT, .triggerEV = DAC_NO_TRIGGER_EV, .outputBuffer = DAC_OBUFFER_EN};
//		DACxHandle.DACxPtr = DAC1;
//		DACxHandle.DACxConfigPtr = &DAC_CH2_config;
//		DAC_init(&DACxHandle);
//	}

	DAC_init_channel(DAC_channel);
	
	/*initilize TIM6,TIM7*/
	TIM_Config_t TIMConfig = {.reloadVal = 1,.prescaler = TIM6_PRESCALE_VAL};
	TIM_Handle_t TIM6Handle = {TIM6,&TIMConfig};
	TIM_init(&TIM6Handle);
	TIMConfig.prescaler = TIM7_PRESCALE_VAL;
	TIM_Handle_t TIM7Handle = {TIM7,&TIMConfig};
	TIM_init(&TIM7Handle);
	
	/*enable TIM6,TIM7 update event interrupt and enable TIM6,TIM7 interrupt vector in NVIC*/
	TIM_interrupt_ctr(TIM6,ENABLE);
	TIM_interrupt_ctr(TIM7,ENABLE);
	TIM_intrpt_vector_ctr(IRQ_TIM6_DAC,ENABLE);
	TIM_intrpt_vector_ctr(IRQ_TIM7,ENABLE);
}

void buzzer_play_sound (uint32_t freq, uint32_t duration){
	uint16_t TIM6reloadVal = 0;
	uint16_t TIM7reloadVal = 0;
	uint32_t APB1_clock = RCC_get_PCLK_value(APB1);
	
	TIM7_flag = 0;
	
	TIM6reloadVal = APB1_clock/(freq*SAMPLE_NUM*(TIM6_PRESCALE_VAL+1))-1;
	TIM7reloadVal =	((APB1_clock/(TIM7_PRESCALE_VAL+1))*duration)/1000 -1;
	
	TIM_set_reload_val(TIM6,TIM6reloadVal);
	TIM_set_reload_val(TIM7,TIM7reloadVal);
	
	TIM_ctr(TIM6,START);
	TIM_ctr(TIM7,START);
	
	while(!TIM7_flag);
}

void buzzer_stop_sound (void)
{
	TIM_ctr(TIM6,STOP);
	TIM_ctr(TIM7,STOP);
}

void TIM6_DAC_IRQHandler (void)
{
	TIM_intrpt_handler(TIM6);
	DAC_write(&DACxHandle,sineWaveTable[count++]);
	if(count == SAMPLE_NUM){
		count = 0;
	}
}

void TIM7_IRQHandler (void)
{
	TIM_intrpt_handler(TIM7);
	TIM_ctr(TIM6,STOP);
	TIM_ctr(TIM7,STOP);
	TIM7_flag = 1;
}
