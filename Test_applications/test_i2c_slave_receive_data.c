/**
*@brief test STM32F4xx I2C slave mode by sending and receiving data between STM32F4xx(slave) and Arduino(master)
*
*This program perform data transmission and reception between STM32F4xx and Arduino through I2C1 peripheral whenever button on Arduino board is press. Purpose is to test I2C slave driver APIs.
*Procedure of data transmission:
*First,STM32F4xx (slave) receive command code from Arduino (master) requesting for length of message
*Then, STM32F4xx send length
*Next, STM32F4xx receive command code from Arduino requesting for message 
*Finally, STM32F4xx send message
*
*I2C configuration: 
*	SCL = 100KHz (Standard mode) 
*	STM32F4xx is slave, Arduino is master
*
*@author Tran Thanh Nhan
*@date 08/14/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*User_button PA0
*Green led PD12
*Red led PD14
*Blue led PD15
*I2C1_SCL PB6
*I2C1_SDA PB7
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_i2c.h"
#include "../Device_drivers/inc/led.h"
#include "string.h"
#include "stdlib.h"

I2C_Handle_t I2C1Handle;

char *msg = "STM32F4xx sending message\n";

void delay (void)
{
	for (int i = 0;i < 3200000;i++){
	}
}

void I2C1_GPIO_pin_init (void)
{	
	GPIO_Pin_config_t GPIO_I2C_pin_config = {.mode = GPIO_MODE_ALTFN,.speed = GPIO_OUTPUT_HIGH_SPEED,.outType = GPIO_OUTPUT_TYPE_OD,.puPdr = GPIO_PU,.altFunc = 4};
	GPIO_Handle_t GPIO_I2C_pin_handle;
	GPIO_I2C_pin_handle.GPIOxPtr = GPIOB;
	
	/*SCL*/
	GPIO_I2C_pin_config.pinNumber = GPIO_PIN_NO_6;
	GPIO_I2C_pin_handle.GPIO_Pin_config = GPIO_I2C_pin_config;
	GPIO_init(&GPIO_I2C_pin_handle);
	
	/*SDA*/
	GPIO_I2C_pin_config.pinNumber = GPIO_PIN_NO_7;
	GPIO_I2C_pin_handle.GPIO_Pin_config = GPIO_I2C_pin_config;
	GPIO_init(&GPIO_I2C_pin_handle);	
}

void I2C1_init (void)
{
	I2C_Config_t I2C1Config = {.SCLspeed = I2C_FSCL_SM,.deviceAddress = 0x33};
	I2C1Handle.I2CxPtr = I2C1;
	I2C1Handle.I2CxConfigPtr = &I2C1Config; 
	I2C_init(&I2C1Handle);
}

int main (void)
{
	/*initilize green led on PD12, red led on PD14, blue led on PD15*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	led_init(GPIOD,GPIO_PIN_NO_14);
	led_init(GPIOD,GPIO_PIN_NO_15);
	
	/*initilize I2C1 on PB6:PB7*/
	I2C1_GPIO_pin_init();
	I2C1_init();
	I2C_periph_ctr(I2C1,ENABLE);
	I2C_ACK_ctr(I2C1,ENABLE);
	
	/*enable I2C1 event and error interrupt vector in NVIC*/
	I2C_intrpt_ctrl(IRQ_I2C1_EV,ENABLE);
	I2C_intrpt_ctrl(IRQ_I2C1_ER,ENABLE);
	
	/*set I2C1 event and error interrupt enable bit*/
	I2C_intrpt_ENbit_ctrl(I2C1,SET);
		
	while(1);
}

void I2C1_EV_IRQHandler(void)
{
	I2C_event_intrpt_handler (&I2C1Handle);
}	

void I2C1_ER_IRQHandler(void)
{
	I2C_err_intrpt_handler (&I2C1Handle);
}

void I2C_application_event_callback (I2C_Handle_t *I2CxHandlePtr,uint8_t event)
{	
	static uint8_t count=0;
	static uint8_t commandCode = 0;
	
	if (event == I2C_EV_SLV_DT_REQ){
		if(commandCode == 0x51){
			I2C_slave_send(I2C1,strlen(msg));
		}else if(commandCode == 0x52){
			I2C_slave_send(I2C1,(uint8_t)*(msg+count));
			count++;
		}
	}else if (event == I2C_EV_SLV_READ){
		commandCode = I2C_slave_receive(I2C1);
	}else if (event == I2C_ERR_AF){
		commandCode = 0xFF;
		count = 0;
	}else if(event == I2C_EV_SLV_STOP_DETECTED){
		led_on(GPIOD,GPIO_PIN_NO_15);
	}	
}