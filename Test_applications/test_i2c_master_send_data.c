/**
*@brief test I2C master send data API by sending data from STM32F4xx to Arduino using I2C protocol
*
*This program send data from STM32F4 to Arduino through I2C1 peripheral whenever user button on STM32F4 board is press. Purpose is to test I2C master send data API.
*I2C configuration: 
*	SCL = 100KHz (Standard mode) 
*	STM32F4xx is master, Arduino is slave
*
*@author Tran Thanh Nhan
*@date 08/08/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*User_button PA0
*I2C1_SCL PB6
*I2C1_SDA PB7
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_i2c.h"
#include "string.h"

I2C_Handle_t I2C1Handle;

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
	I2C_Config_t I2C1Config = {.SCLspeed = I2C_FSCL_SM,.ACKctr = I2C_ACKctr_ENABLE,.deviceAddress = 0x33};
	I2C1Handle.I2CxPtr = I2C1;
	I2C1Handle.I2CxConfigPtr = &I2C1Config; 
	I2C_init(&I2C1Handle);
}

void user_button_init (void)
{
	GPIO_Pin_config_t GPIO_PA0_config = {.pinNumber=GPIO_PIN_NO_0,.mode=GPIO_MODE_IN,.puPdr=GPIO_PDR};
	GPIO_Handle_t	GPIO_PA0_handle = {GPIOA,GPIO_PA0_config};
	GPIO_init(&GPIO_PA0_handle);
}	

uint8_t user_button_read (void)
{
	return GPIO_read_pin(GPIOA,GPIO_PIN_NO_0);
}

void green_led_init (void)
{
	GPIO_Pin_config_t GPIO_PD12_config={.pinNumber=GPIO_PIN_NO_12,.mode=GPIO_MODE_OUT,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_PD12_Handle = {GPIOD,GPIO_PD12_config};
	GPIO_init(&GPIO_PD12_Handle);
}

void green_led_on (void)
{
	GPIO_write_pin(GPIOD,GPIO_PIN_NO_12,SET);
}

void green_led_off (void)
{
	GPIO_write_pin(GPIOD,GPIO_PIN_NO_12,CLEAR);
}

int main (void)
{
	
	/*initilize green led on PD12*/
	green_led_init();
	
	/*Initialize user button on PA0*/
	user_button_init();
	
	/*Initilize SPI2 on PB15:PB12*/
	I2C1_GPIO_pin_init();
	I2C1_init();
	I2C_periph_ctr(I2C1,ENABLE);
	
	char *dataPtr = "We are testing I2C master\n";
	
	while(1){
		if(user_button_read()){
			delay();
			green_led_on();
			I2C_master_send(&I2C1Handle,(uint8_t*)dataPtr,strlen(dataPtr),0x68);
		}else{
			green_led_off();
		}
	}
}