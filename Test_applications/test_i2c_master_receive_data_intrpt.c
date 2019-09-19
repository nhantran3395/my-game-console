/**
*@brief test I2C master receive data (interrupt base) API by sending and receiving data between STM32F4xx and Arduino using I2C protocol
*
*This program perform sending and receiving data between STM32F4xx and Arduino through I2C1 peripheral whenever user button on STM32F4 board is press. Purpose is to test I2C master receive data API.
*Procedure of data transmission:
*First, master send command code to ask slave to send length of data
*Then, master receive length
*Next, master send command code to ask slave to send data
*Finally, master receive data
*
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

int main (void)
{
	/*initilize green led on PD12, red led on PD14, blue led on PD15*/
	led_init(GPIOD,GPIO_PIN_NO_12);
	led_init(GPIOD,GPIO_PIN_NO_14);
	led_init(GPIOD,GPIO_PIN_NO_15);
	
	/*initialize user button on PA0*/
	user_button_init();
	
	/*initilize I2C1 on PB6:PB7*/
	I2C1_GPIO_pin_init();
	I2C1_init();
	I2C_periph_ctr(I2C1,ENABLE);
	
	/*enable I2C1 event and error interrupt vector in NVIC*/
	I2C_intrpt_ctrl(IRQ_I2C1_EV,ENABLE);
	I2C_intrpt_ctrl(IRQ_I2C1_ER,ENABLE);
	
	uint8_t LengthCode = 0x51;
	uint8_t DataCode = 0x52;
	char rxLength = 0;
	char *rxData = NULL;
	rxData = malloc(sizeof(uint8_t)*50);
	
	while(1){
		if(user_button_read()){
			delay();
			led_on(GPIOD,GPIO_PIN_NO_12);
			while(I2C_master_send_intrpt(&I2C1Handle,&LengthCode,1,0x68,I2C_REPEATED_START_DISABLE)!=I2C_READY);
			while(I2C_master_receive_intrpt(&I2C1Handle,(uint8_t*)&rxLength,1,0x68,I2C_REPEATED_START_DISABLE)!=I2C_READY);
			while(I2C_master_send_intrpt(&I2C1Handle,&DataCode,1,0x68,I2C_REPEATED_START_DISABLE)!=I2C_READY);
			while(I2C_master_receive_intrpt(&I2C1Handle,(uint8_t*)rxData,rxLength,0x68,I2C_REPEATED_START_DISABLE)!=I2C_READY);
		}else{
			led_off(GPIOD,GPIO_PIN_NO_12);
		}
	}
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
	if (event == I2C_EV_MST_TX_CMPLT){
		led_on(GPIOD,GPIO_PIN_NO_15);
	}else if (event == I2C_EV_MST_RX_CMPLT){
		led_on(GPIOD,GPIO_PIN_NO_15);
	}else if (event == I2C_ERR_AF){
		led_on(GPIOD,GPIO_PIN_NO_14);
	}
}