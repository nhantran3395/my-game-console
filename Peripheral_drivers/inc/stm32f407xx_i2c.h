/**
*@file stm32f407xx_i2c.h
*@brief provide APIs for I2C communications.
*
*This header file provide APIs for communication between stm32f407xx MCU and I/O devices through I2C protocol.
*
*@author Tran Thanh Nhan
*@date 31/07/2019
*/

#ifndef STM32F407XX_I2C_H
#define STM32F407XX_I2C_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include "stm32f407xx_rcc.h"
#include <stdint.h>
#include <stdlib.h>

/*
*@I2C_SCLspeed
*I2C SCL frequency selection
*/
#define I2C_FSCL_SM 100000
#define I2C_FSCL_FM 400000

/*
*@I2C_ACKctr
*I2C ACKing enable/disable
*/
#define I2C_ACKctr_DISABLE	0
#define I2C_ACKctr_ENABLE	1 

/*
*@I2C_FMdutyCycle
*I2C fast mode duty cycle selection
*/
#define I2C_FMduty_2	0
#define I2C_FMduty_16_9	1 

/*
*@I2C_RepeatedStart
*I2C repeated start enable/disable
*/
#define I2C_REPEATED_START_DISABLE 0
#define I2C_REPEATED_START_ENABLE 1 

/*
*@I2C_State
*State of I2C peripheral
*/
#define I2C_READY 0
#define I2C_BUSY_IN_TX 1
#define I2C_BUSY_IN_RX 2

/*
*@I2C_Event_and_Error
*I2C interrupt event and error macro
*/
#define I2C_EV_MST_TX_CMPLT	0
#define I2C_EV_MST_RX_CMPLT	1
#define I2C_EV_SLV_STOP_DETECTED	2
#define	I2C_ERR_BERR 3
#define	I2C_ERR_ARLO 4
#define	I2C_ERR_AF 5
#define	I2C_ERR_OVR 6
#define	I2C_ERR_TIMEOUT 7
#define I2C_EV_SLV_DT_REQ	8/*device (acting as slave) is requested to send data*/
#define I2C_EV_SLV_READ 9	/*device (acting as slave) needed to read data*/

typedef struct{
	uint32_t SCLspeed;	/*refer to @I2C_SCLspeed for possible value*/
	uint8_t deviceAddress;	
	uint8_t ACKctr;	/*refer to @I2C_ACKctr for possible value*/	
	uint8_t FMdutyCycle;	/*refer to @I2C_FMdutyCycle for possible value*/	
}I2C_Config_t;

typedef struct{
	I2C_TypeDef *I2CxPtr;
	I2C_Config_t *I2CxConfigPtr;
	uint8_t *txBufferPtr;	/*To store application TxBuffer address*/
	uint8_t *rxBufferPtr;	/*To store application RxBuffer address*/
	uint32_t txLength;	/*To store Tx Length*/
	uint32_t rxLength;	/*To store Rx Length*/
	uint8_t State;	/*To store I2C peripheral state: BUSY_IN_TX or BUSY_IN_RX or READY*/
	uint8_t slaveAddr;	/*To store Slave device address*/
	uint32_t rxSize;
	uint8_t repeatedStart;
}I2C_Handle_t;

/**
*@brief I2C peripheral clock enable/disable
*@param Pointer to base address of I2C registers
*@param Enable or disable action
*@return none
*/
void I2C_CLK_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis);

/**
*@brief I2C peripheral enable/disable
*
*Set or clear PE bit in CR1 register to enable/disable I2C peripheral
*Call this function after I2C_init to enable I2C communication. 
*
*@param Pointer to base address of I2C registers
*@param Enable or disable action
*@return none
*/
void I2C_periph_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis);

/**
*@brief I2C peripheral ACKing enable/disable
*
*Set or clear ACK bit in CR1 register to enable/disable ACKing
*
*@param Pointer to base address of I2C registers
*@param Enable or disable action
*@return none
*/
void I2C_ACK_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis);

/**
*@brief Set/clear I2C interrupt enable bit 
*
*Set or clear ITBUFEN, ITEVTEN, ITERREN in CR2
*
*@param Pointer to base address of I2C registers
*@param Set or clear action
*@return none
*/
void I2C_intrpt_ENbit_ctrl(I2C_TypeDef *I2CxPtr,uint8_t setOrClear);

/**
*@brief Initialize I2C communication
*@param Pointer to I2C Handle struct
*@return none
*/
void I2C_init(I2C_Handle_t *I2CxHandlePtr);

/**
*@brief Deinitialize I2C communication
*@param Pointer to I2C_Handle struct
*@return none
*/
void I2C_deinit(I2C_TypeDef *I2CxPtr);

/**
*@brief Master receive data
*@param Pointer to I2C handle struct
*@param Pointer to memory region to store received data
*@param Length of data
*@param Slave address
*@param Enable or disable repeated start
*@return none
*/
void I2C_master_receive (I2C_Handle_t *I2CxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length,uint8_t slaveAddr,uint8_t repeatedStart);

/**
*@brief Master send data 
*@param Pointer to I2C handle struct
*@param Pointer to data to send
*@param Length of data
*@param Slave address
*@param Enable or disable repeated start
*@return none
*/
void I2C_master_send(I2C_Handle_t *I2CxHandlePtr, uint8_t *txBufferPtr, uint32_t Length, uint8_t slaveAddr,uint8_t repeatedStart);

/**
*@brief Receive data from I2C bus (interrup base)
*@param Pointer to I2C handle struct
*@param Pointer to memory region to store received data
*@param Length of data
*@param Slave address
*@param Enable or disable repeated start
*@return Status of Rx
*/
uint8_t I2C_master_receive_intrpt (I2C_Handle_t *I2CxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length,uint8_t slaveAddr,uint8_t repeatedStart);

/**
*@brief Send data to SPI bus (interrupt base)
*@param Pointer to SPI handle struct
*@param Pointer to data to send
*@param Length of data
*@param Slave address
*@param Enable or disable repeated start
*@return Status of Tx
*/
uint8_t I2C_master_send_intrpt (I2C_Handle_t *I2CxHandlePtr, uint8_t *txBufferPtr, uint32_t Length,uint8_t slaveAddr,uint8_t repeatedStart);

/**
*@brief Slave receive data
*@param Pointer to base address of I2C registers
*@return Received data
*/
uint8_t I2C_slave_receive (I2C_TypeDef *I2CxPtr);

/**
*@brief Slave send data 
*@param Pointer to base address of I2C registers
*@param Data byte
*@return none
*/
void I2C_slave_send(I2C_TypeDef *I2CxPtr, uint8_t dataByte);

/**
*@brief Enable or disable I2C 's interrupt 
*@param IRQ number
*@param Enable or disable action
*@return none
*/
void I2C_intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis);

/**
*@brief Config interrupt priority for I2C peripheral
*@param IRQ number
*@param Priority
*@return none
*/
void I2C_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority);

/**
*@brief I2C error interrupt handler
*@param Pointer to I2C_Handle struct
*@return none
*/
void I2C_err_intrpt_handler (I2C_Handle_t *I2CxHandlePtr);

/**
*@brief I2C event interrupt handler
*@param Pointer to I2C_Handle struct
*@return none
*/
void I2C_event_intrpt_handler (I2C_Handle_t *I2CxHandlePtr);

/**
*@brief inform application of I2C event or error
*@param Pointer to I2C_Handle struct
*@param Event or error
*@return none
*/
void I2C_application_event_callback (I2C_Handle_t *I2CxHandlePtr,uint8_t event);
#endif 
