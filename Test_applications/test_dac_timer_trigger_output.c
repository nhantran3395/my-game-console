/**
*@brief test STM32F4xx DAC driver APIs 
*
*This generate a 100Hz sivewave on DAC channel 2 (PA5).
*Logic analyzer is then used to monitor the generated sinewave.
*Purpose of this program is to confirm correctness of DAC driver APIs.
*
*@author Tran Thanh Nhan
*@date 08/19/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*DAC_channel_2 PA5
*Green_led PD12
*User_buttin PA0
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_dac.h"
#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/button.h"
#include "string.h"
#include "stdlib.h"

#define SAMPLE_NUM 10
uint16_t sineWaveTable[SAMPLE_NUM] = {2048,3251,3995,3996,3253,2051,847,101,98,839};
 
uint16_t count = 0;
uint8_t writeCmplt = CLEAR;

DAC_Handle_t DAC_CH2_Handle;

void DAC_CH2_GPIO_pin_init (void)
{	
	GPIO_Pin_config_t GPIO_DAC_CH2_pin_config = {.mode = GPIO_MODE_ANL,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr = GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_DAC_CH2_pin_handle;
	GPIO_DAC_CH2_pin_handle.GPIOxPtr = GPIOA;
	
	GPIO_DAC_CH2_pin_config.pinNumber = GPIO_PIN_NO_5;
	GPIO_DAC_CH2_pin_handle.GPIO_Pin_config = GPIO_DAC_CH2_pin_config;
	GPIO_init(&GPIO_DAC_CH2_pin_handle);	
}

void DAC_CH2_init (void)
{
	static DAC_Config_t DAC_CH2_Config = {.channel = DAC_CHANNEL_2, .resolution = DAC_RES_12_bits, .alignment = DAC_ALIGNMENT_RIGHT, .triggerEV = DAC_TRIGGER_EV_TIM6,.outputBuffer = DAC_OBUFFER_EN};
	DAC_CH2_Handle.DACxPtr = DAC1;
	DAC_CH2_Handle.DACxConfigPtr = &DAC_CH2_Config; 
	DAC_init(&DAC_CH2_Handle);
}

void TIM6_init(void)
{
	/*select reload value and prescaler so that TIM6 period is 500ms*/
	TIM_Config_t TIM6Config = {.reloadVal = 999,.prescaler = 7999};
	TIM_Handle_t TIM6Handle = {TIM6,&TIM6Config};
	TIM_init(&TIM6Handle);
}

int main (void){
	/*initilize green led on PD12*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	
	/*initialize DAC channel 2 on PA5*/
	DAC_CH2_GPIO_pin_init();
	DAC_CH2_init();
	
	/*initialize TIM6, enable interrupt on TIM6 's update event, enable TIM6 interrupt vector in NVIC*/
	TIM6_init();
	TIM_interrupt_ctr(TIM6,ENABLE);
	TIM_intrpt_vector_ctr(IRQ_TIM6_DAC,ENABLE);
	
	/*start TIM6*/
	TIM_ctr(TIM6,START);
	
	/*enable trigger TRGO on update event*/
	TIM_update_event_TRGO(TIM6);
	
	while(1){
		if(writeCmplt != SET){
			DAC_write(&DAC_CH2_Handle,sineWaveTable[count++]);
			writeCmplt = SET;
		}
		if(count == SAMPLE_NUM){
			count = 0;
		}
	}
}

void TIM6_DAC_IRQHandler (void)
{
	TIM_intrpt_handler(TIM6);
	led_toggle(GPIOD,GPIO_PIN_NO_12);
	writeCmplt = CLEAR;
}
