/**
*@file stm32f407xx_spi.c
*@brief provide functions for interfacing with SPI peripherals.
*
*This source file provide functions for communication between stm32f407xx MCU and I/O devices through SPI protocol.
*
*@author Tran Thanh Nhan
*@date 28/07/2019
*
*@reference	https://stm32f4-discovery.net/2014/04/library-05-spi-for-stm32f4xx/
*@reference	Mastering microcontroller with embedded driver development course on Udemy		
*/

/**
*@Version 1.0 
*28/07/2019
*/

/**
*@Version 1.1 
*30/08/2019
*Add SPI_general_init function
*Add SPI_send_8_bits function
*Add SPI_send_16_bits function
*/

#ifndef STM32F407XX_SPI_H
#define STM32F407XX_SPI_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include "stm32f407xx_gpio.h"
#include <stdint.h>
#include <stdlib.h>

/***********************************************************************
Macro definition
***********************************************************************/

/*
*@SPI_DEVICE_MODE
*SPI device mode configuration
*/
#define SPI_MODE_SLAVE 0
#define SPI_MODE_MASTER 1

/*
*@SPI_BUS_CONFIG
*SPI bus mode configuration
*/
#define SPI_BUS_FULL_DUPLEX 0
#define SPI_BUS_HALF_DUPLEX 1
#define SPI_BUS_SIMPLEX_RXONLY 2

/*
*@SPI_DATA_FRAME
*SPI data frame configuration
*/
#define SPI_DATA_8BITS 0
#define SPI_DATA_16BITS 1

/*
*@SPI_CLK_PHASE
*SPI clock phase configuration
*/
#define SPI_CLK_PHASE_1ST_E 0
#define SPI_CLK_PHASE_2ND_E 1

/*
*@SPI_CLK_POL
*SPI clock polarity configuration
*/
#define SPI_CLK_POL_LIDLE 0
#define SPI_CLK_POL_HIDLDE 1
/*
*@SPI_SSM_CTR
*SPI slave software management enable/disable
*/
#define SPI_SSM_DIS 0
#define SPI_SSM_EN 1
/*
*@SPI_CLK_SPEED
*SPI clock speed configuration
*/
#define SPI_CLK_SPEED_DIV2 0
#define SPI_CLK_SPEED_DIV4 1
#define SPI_CLK_SPEED_DIV8 2
#define SPI_CLK_SPEED_DIV16 3
#define SPI_CLK_SPEED_DIV32 4
#define SPI_CLK_SPEED_DIV64 5
#define SPI_CLK_SPEED_DIV128 6
#define SPI_CLK_SPEED_DIV256 7

/*
*@SPI_COMMUNICATION_STATE
*Possible SPI communication state
*/
#define	SPI_STATE_READY 0
#define SPI_STATE_TX_BUSY 1
#define SPI_STATE_RX_BUSY 2

/*
*@SPI_EVENT
*Event during SPI communication
*/
#define SPI_EV_TRANSMISSION_CMPLT 1
#define SPI_EV_RECEPTION_CMPLT	2

/***********************************************************************
SPI structure and enumeration definition
***********************************************************************/
typedef struct{
	uint8_t deviceMode;	/*Refer to @SPI_DEVICE_MODE for possible value*/
	uint8_t busConfig;	/*Refer to @SPI_BUS_CONFIG for possible value*/
	uint8_t dataFrame;	/*Refer to @SPI_DATA_FRAME for possible value*/
	uint8_t clkPhase;	/*Refer to @SPI_CLK_PHASE for possible value*/
	uint8_t clkPol;	/*Refer to @SPI_CLK_POL for possible value*/
	uint8_t swSlaveManage;	/*Refer to @SPI_SSM_CTR for possible value*/
	uint8_t clkSpeed;	/*Refer to @SPI_CLK_SPEED for possible value*/
}SPI_Config_t;

typedef struct{
	SPI_TypeDef *SPIxPtr;
	SPI_Config_t *SPIxConfigPtr;
	uint8_t *txBufferPtr;	/*To store application TxBuffer address*/
	uint8_t *rxBufferPtr;	/*To store application RxBuffer address*/
	uint32_t txLength;	/*To store Tx Length*/
	uint32_t rxLength;	/*To store Rx Length*/
	uint8_t txState;	/*To store Tx State: BUSY_IN_TX or READY*/
	uint8_t rxState;	/*To store Rx State: BUSY_IN_RX or READY*/
}SPI_Handle_t;

/*
						|pins pack 1            			|pin packs 2           				|pin packs 3
SPIX    	|MOSI   MISO    SCK     |MOSI   MISO    SCK     |MOSI   MISO    SCK
SPI1    	|PA7    PA6     PA5     	|PB5    PB4     PB3     	|
SPI2    	|PC3    PC2     PB10    	|PB15   PB14    PB13    |PI3    PI2     PI1
SPI3    	|PB5    PB4     PB3     	|PC12   PC11    PC10    |
*/
typedef enum{
	SPI_pins_pack_1,
	SPI_pins_pack_2,
	SPI_pins_pack_3
}SPI_pins_pack_t;

/***********************************************************************
SPI driver function prototype
***********************************************************************/

/**
*@brief 		SPI clock enable/disable
*@param 	Pointer to base address of SPI registers
*@param 	Enable or disable action
*@return 	None
*/
void SPI_CLK_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis);

/**
*@brief 		SPI peripheral enable/disable
*@param 	Pointer to base address of SPI registers
*@param 	Enable or disable action
*@return 	None
*/
void SPI_periph_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis);

/**
*@brief 		Enable use of NSS pin in Hardware Slave Management (SSM = 0) mode, in case MCU is used as Master. 
*
*This set/clear SSOE bit in CR2 register. When SSM = 0 and SSOE =1, NSS pin is driven by hardware. NSS is driven low when SPI is enabled and high when SPI is disabled. 
*User need to call this function after initilizing single Master - single Slave SPI communication with Master mode, Hardware Slave Management configuration.
*
*@param 	Pointer to base address of SPI registers
*@param 	Enable or disable action
*@return 	None
*/
void SPI_NSS_pin_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis);

/**
*@brief 		 
*@param 	Pointer to base address of SPI registers
*@param 	Enable or disable action
*@return 	None
*/
void SPI_SSI_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis);

/**
*@brief 		Check whether SPI is busy in communication or not 
*
*This read bit BSY in CR2 register. 
*User need to call this function to check SPI state before closing(disabling) SPI peripheral.
*
*@param 	Pointer to base address of SPI registers
*@return 	Value of BSY bit
*/
uint8_t SPI_busy_check(SPI_TypeDef *SPIxPtr);

/**
*@brief 		Initialize SPI peripheral
*@param 	Pointer to SPI handle struct
*@return 	None
*/
void SPI_init(SPI_Handle_t *SPIxHandlePtr);

/**
*@brief 		Initialize SPI peripheral and initialize corresponding GPIO pins as SPI function
*@param 	Pointer to SPIx peripheral
*@param 	Pins pack
*@param 	Device mode
*@param 	Bus configuration
*@param 	Data frame
*@param 	Clock phase
*@param 	Clock polarity
*@param 	Software slave manager
*@param 	Clock speed
*@return 	Pointer to SPI handle struct
*/
SPI_Handle_t* SPI_general_init(SPI_TypeDef *SPIxPtr, SPI_pins_pack_t pinsPack, uint32_t deviceMode, uint8_t busConfig, uint8_t dataFrame, uint8_t clkPhase, uint8_t clkPol, uint8_t swSlaveManage, uint8_t clkSpeed);

/**
*@brief		 	Deinitialize SPI peripheral
*@param 	Pointer to SPI handle struct
*@return 	None
*/
void SPI_deinit(SPI_TypeDef *SPIxPtr);

/**
*@brief			Receive multiple bytes from SPI 
*
*This receive 8 or 16 bits at a time  based on data frame configuration at initilization  
*
*@param 	Pointer to base address of SPI registers
*@param 	Pointer to buffer to store data bytes
*@param 	Number of bytes to receive
*@return 	None
*/
void SPI_receive_data (SPI_TypeDef *SPIxPtr, uint8_t *rxBufferPtr,uint32_t Length);

/**
*@brief 		Send multiple bytes through SPI
*
*This send 8 or 16 bits at a time  based on data frame configuration at initilization  
*
*@param 	Pointer to base address of SPI registers
*@param 	Pointer to buffer containing data bytes to send
*@param 	Number of bytes to send
*@return 	None
*/
void SPI_send_data(SPI_TypeDef *SPIxPtr, uint8_t *txBufferPtr, uint32_t Length);

/**
*@brief 		Send 1 byte of data through SPI using 8 bits data frame configuration  
*@param	Pointer to base address of SPI registers
*@param	Pointer to data to send
*@param 	1 byte of data 
*@return 	None
*/
void SPI_send_8_bits(SPI_TypeDef *SPIxPtr, uint8_t data);

/**
*@brief 		Send 2 byte (16 bits) of data at a time through SPI using 16 bits data frame configuration 
*@param 	Pointer to base address of SPI registers
*@param 	Pointer to data to send
*@param 	Length of data
*@return		None
*/
void SPI_send_16_bits(SPI_TypeDef *SPIxPtr, uint16_t data);

/**
*@brief 		Receive multiple bytes from SPI (interrup base)
*@param 	Pointer to SPI handle struct
*@param 	Pointer to buffer to store received data
*@param 	Length of data
*@return 	Status of receiver
*/
uint8_t SPI_receive_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length);

/**
*@brief 		Send multiple bytes to SPI (interrupt base)
*@param 	Pointer to SPI handle struct
*@param 	Pointer to buffer containing data to send
*@param 	Length of data
*@return 	Status of transmitter
*/
uint8_t SPI_send_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *txBufferPtr, uint32_t Length);

/**
*@brief			Enable or disable SPI 's interrupt  request in NVIC
*@param 	IRQ number
*@param 	Enable or disable action
*@return 	None
*/
void SPI_intrpt_vector_ctrl (uint8_t IRQnumber, uint8_t enOrDis);

/**
*@brief 		Config interrupt priority for SPI peripheral
*@param 	IRQ number
*@param 	Priority
*@return 	None
*/
void SPI_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority);

/**
*@brief 		General Interrupt handler for SPI peripheral
*@param 	Pointer to SPI handle struct
*@return 	None
*/
void SPI_intrpt_handler (SPI_Handle_t *SPIxHandlePtr);

/**
*@brief 		Inform application of SPI event or error
*@param 	Pointer to SPI handle struct
*@param 	Event/error macro
*@return 	None
*/
void SPI_application_event_callback (SPI_Handle_t *SPIxHandlePtr,uint8_t event);
#endif 
