/**
*@file stm32f407xx_i2c.c
*@brief provide APIs for I2C communications.
*
*This source file provide APIs for communication between stm32f407xx MCU and I/O devices through I2C protocol.
*
*@author Tran Thanh Nhan
*@date 31/07/2019
*/

#include "../inc/stm32f407xx_i2c.h"

//extern int32_t RCC_get_PCLK_value(uint8_t APBx);
//extern int32_t RCC_get_PLL_output (void);

/***********************************************************************
Private function: generate start/ stop condition 
***********************************************************************/
static void I2C_start_stop_generation(I2C_TypeDef *I2CxPtr, uint8_t startOrStop)
{	
	if(startOrStop == START)
	{
		I2CxPtr->CR1 |= I2C_CR1_START;
	}else{
		I2CxPtr->CR1 |= I2C_CR1_STOP;
	}			
}

/***********************************************************************
Private function: execute address phase (send 7-bit address and R/NW bit)
***********************************************************************/
static void I2C_address_phase_execute(I2C_TypeDef *I2CxPtr, uint8_t slaveAddr, uint8_t readOrWrite)
{
	uint8_t sendData = slaveAddr << 1;
	
	if(readOrWrite == READ){
		sendData |= 1;
	}else{
		sendData &= ~1;
	}
	
	I2CxPtr->DR = sendData;
}

/***********************************************************************
Private function: clear ADDR flag in SR register
@Note: in receiver mode, in case only 1 byte has to be received, ACKing is disabled before clearing ADDR flag
***********************************************************************/
static void I2C_clear_ADDRflag(I2C_Handle_t *I2CxHandlePtr)
{
	if(I2CxHandlePtr->State == I2C_BUSY_IN_RX){
		if(I2CxHandlePtr->rxLength == 1){
			I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
		}
	}
	
	/*clear ADDR flag*/
	uint16_t tempData = I2CxHandlePtr->I2CxPtr->SR1;
	tempData = I2CxHandlePtr->I2CxPtr->SR2;
	(void) tempData;
}

/***********************************************************************
Private function: clear STOPF flag in SR register
***********************************************************************/
static void I2C_clear_STOPFflag(I2C_TypeDef *I2CxPtr)
{
	uint16_t tempData = I2CxPtr->SR1;
	(void) tempData;
	I2CxPtr->CR1 |= 0;
}

/***********************************************************************
Private function: read data from DR register
***********************************************************************/
static void I2C_read_data(I2C_TypeDef *I2CxPtr,uint8_t *rxBufferPtr, uint32_t *LengthPtr)
{
	while(!(I2CxPtr->SR1 & I2C_SR1_RXNE));
	*rxBufferPtr = I2CxPtr->DR ;
	(*LengthPtr)--;
}

/***********************************************************************
Private function: close data reception
***********************************************************************/
static void I2C_close_receive_data(I2C_Handle_t *I2CxHandlePtr)
{
	I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CR2_ITBUFEN);
	I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CR2_ITEVTEN);
	I2CxHandlePtr->rxBufferPtr = NULL;
	I2CxHandlePtr->rxLength = 0;
	I2CxHandlePtr->State = I2C_READY;
	I2CxHandlePtr->slaveAddr = 0;
	I2CxHandlePtr->repeatedStart = DISABLE;
}

/***********************************************************************
Private function: close data trasmission
***********************************************************************/
static void I2C_close_send_data(I2C_Handle_t *I2CxHandlePtr)
{
	I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CR2_ITBUFEN);
	I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CR2_ITEVTEN);
	I2CxHandlePtr->txBufferPtr = NULL;
	I2CxHandlePtr->txLength = 0;
	I2CxHandlePtr->State = I2C_READY;
	I2CxHandlePtr->slaveAddr = 0;
	I2CxHandlePtr->repeatedStart = DISABLE;
}

/***********************************************************************
Private function: write data to DR register
***********************************************************************/
static void I2C_send_data(I2C_TypeDef *I2CxPtr,uint8_t *txBufferPtr, uint32_t *LengthPtr)
{
	while(!(I2CxPtr->SR1 & I2C_SR1_TXE));
	I2CxPtr->DR = *txBufferPtr;
	(*LengthPtr)--;
}

/***********************************************************************
I2C clock enable/disable
***********************************************************************/
void I2C_CLK_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE)
	{
		if(I2CxPtr == I2C1){
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		}else if(I2CxPtr == I2C2){
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		}else if(I2CxPtr == I2C3){
			RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}	
	}else{
		if(I2CxPtr == I2C1){
			RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN);
		}else if(I2CxPtr == I2C2){
			RCC->APB1ENR &= ~(RCC_APB1ENR_I2C2EN);
		}else if(I2CxPtr == I2C3){
			RCC->APB1ENR &= ~(RCC_APB1ENR_I2C3EN);
		}	
	}
}

/***********************************************************************
I2C perpheral enable/disable
***********************************************************************/
void I2C_periph_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		I2CxPtr->CR1 |= I2C_CR1_PE;
	}else{
		I2CxPtr->CR1 &= ~(I2C_CR1_PE);
	}
}


/***********************************************************************
enable/disable ACKing 
***********************************************************************/
void I2C_ACK_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		I2CxPtr->CR1 |= I2C_CR1_ACK;
	}else{
		I2CxPtr->CR1 &= ~(I2C_CR1_ACK);
	}
}

/***********************************************************************
Set/clear I2C interrupt enable bit 
***********************************************************************/
void I2C_intrpt_ENbit_ctrl(I2C_TypeDef *I2CxPtr,uint8_t setOrClear)
{
	if(setOrClear == SET){
		I2CxPtr->CR2 |= I2C_CR2_ITBUFEN;
		I2CxPtr->CR2 |= I2C_CR2_ITEVTEN;
		I2CxPtr->CR2 |= I2C_CR2_ITERREN;
	}else{
		I2CxPtr->CR2 &= ~(I2C_CR2_ITBUFEN);
		I2CxPtr->CR2 &= ~(I2C_CR2_ITEVTEN);
		I2CxPtr->CR2 &= ~(I2C_CR2_ITERREN);	
	}
}

/***********************************************************************
Initialize I2C communication
***********************************************************************/
void I2C_init(I2C_Handle_t *I2CxHandlePtr)
{
	/*enable clock for I2Cx peripheral*/
	I2C_CLK_ctr(I2CxHandlePtr->I2CxPtr,ENABLE);
	
	/*disable I2Cx peripheral for initilization*/
	I2C_periph_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
	
	/*set I2C peripheral input frequency*/
	uint32_t fPCLK1 = RCC_get_PCLK_value(APB1);
	I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CR2_FREQ);
	I2CxHandlePtr->I2CxPtr->CR2 |=	(fPCLK1/1000000) << I2C_CR2_FREQ_Pos;
	
	/*config I2C SCL line speed (standard mode or fast mode). if fast mode, config duty cycle*/
	I2CxHandlePtr->I2CxPtr->CCR &= ~(I2C_CCR_FS);
	I2CxHandlePtr->I2CxPtr->CCR |= I2CxHandlePtr->I2CxConfigPtr->SCLspeed << I2C_CCR_FS_Pos;
	
	uint16_t CCRval = 0;
	
	if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_SM){
		/*slow mode*/
		CCRval = fPCLK1/(2*I2C_FSCL_SM);
	}else if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed > I2C_FSCL_SM && I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_FM){
		/*fast mode*/		
		if(I2CxHandlePtr->I2CxConfigPtr->FMdutyCycle == I2C_FMduty_2){
			/*t_low:t_high 2:1*/
			I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CCR_DUTY);
			CCRval = fPCLK1/(3*I2C_FSCL_FM);
		}else{
			/*t_low:t_high 2:1 16:9*/
			I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CCR_DUTY;
			CCRval = fPCLK1/(25*I2C_FSCL_FM);
		}
	}
	 
	I2CxHandlePtr->I2CxPtr->CCR &= ~(I2C_CCR_CCR);
	I2CxHandlePtr->I2CxPtr->CCR |= CCRval << I2C_CCR_CCR_Pos;
	
	/*program device own address*/ 
	I2CxHandlePtr->I2CxPtr->OAR1	&= ~(I2C_OAR1_ADD1_7);
	I2CxHandlePtr->I2CxPtr->OAR1	|= I2CxHandlePtr->I2CxConfigPtr->deviceAddress <<	I2C_OAR1_ADD1_Pos;
	
	/*config t_rise value*/
	uint8_t regVal = 0;
	uint16_t tRiseMax = 0;
	
	if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_SM){
		tRiseMax = 1000;
	}else if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed > I2C_FSCL_SM && I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_FM){
		tRiseMax = 300;
	}
	
	regVal = ((double)tRiseMax/1000000000)*fPCLK1+1;
	I2CxHandlePtr->I2CxPtr->TRISE = (regVal&0x3F);
}

/***********************************************************************
Deinitialize I2C communication
***********************************************************************/
void I2C_deinit(I2C_TypeDef *I2CxPtr)
{
	if(I2CxPtr == I2C1){
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_I2C1RST);
	}else if(I2CxPtr == I2C2){
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_I2C2RST);
	}else if(I2CxPtr == I2C3){
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C3RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_I2C3RST);
	}
}

/***********************************************************************
Master receive data 
***********************************************************************/
void I2C_master_receive (I2C_Handle_t *I2CxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length,uint8_t slaveAddr,uint8_t repeatedStart)
{
	/*enable acking for master*/
	I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr, ENABLE);
	
	/*generate start condition and wait for SB flag to be set*/
	I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,START);
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_SB));
	
	/*execute addressing phase and wait for ADDR flag to be set*/
	I2C_address_phase_execute(I2CxHandlePtr->I2CxPtr,slaveAddr,READ);
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_ADDR));
	
	if(Length == 1){
		/*case slave only send 1 byte of data, disable ACK, clear ADDR flag,generate stop condition before reading data*/
		I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
		I2C_clear_ADDRflag(I2CxHandlePtr);
		if(repeatedStart == I2C_REPEATED_START_DISABLE){
			I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,STOP);
		}
		I2C_read_data(I2CxHandlePtr->I2CxPtr, rxBufferPtr, &Length);
	}else{
		/*case slave send multiple data bytes, clear ADDR flag*/
		I2C_clear_ADDRflag(I2CxHandlePtr);
		
		while(Length){
			if(Length == 1){
				/*when there are 2 bytes left to receive, disable ACK and generate stop condition*/
				I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
				if(repeatedStart == I2C_REPEATED_START_DISABLE){
					I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,STOP);
				}	
			}
			I2C_read_data(I2CxHandlePtr->I2CxPtr, rxBufferPtr, &Length);
			rxBufferPtr++;
		}
	}
}

/***********************************************************************
Master send data 
***********************************************************************/
void I2C_master_send(I2C_Handle_t *I2CxHandlePtr, uint8_t *txBufferPtr, uint32_t Length, uint8_t slaveAddr, uint8_t repeatedStart)
{
	/*generate start condition and wait for SB flag to be set*/
	I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,START);
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_SB));
	
	/*execute addressing phase*/
	I2C_address_phase_execute(I2CxHandlePtr->I2CxPtr,slaveAddr,WRITE);
	
	/*wait for ADDR flag to be set then clear*/
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_ADDR));
	I2C_clear_ADDRflag(I2CxHandlePtr);
	
	/*send data until length = 0*/
	while(Length){
		I2C_send_data(I2CxHandlePtr->I2CxPtr,txBufferPtr,&Length);
		txBufferPtr++;
	}
	
	/*wait for TXE and BTF flag to be set before generating stop condition*/
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_TXE));
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_BTF));
	
	/*generate stop condition*/
	if(repeatedStart == I2C_REPEATED_START_DISABLE)
	{
		I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,STOP);
	}
}

/***********************************************************************
Master receive data (interrupt base)
***********************************************************************/
uint8_t I2C_master_receive_intrpt (I2C_Handle_t *I2CxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length,uint8_t slaveAddr,uint8_t repeatedStart)
{
	uint8_t state = I2CxHandlePtr->State;
	if(I2CxHandlePtr->State == I2C_READY){
		I2CxHandlePtr->rxBufferPtr = rxBufferPtr;
		I2CxHandlePtr->rxLength = Length;
		I2CxHandlePtr->State = I2C_BUSY_IN_RX;
		I2CxHandlePtr->slaveAddr = slaveAddr;
		I2CxHandlePtr->repeatedStart = repeatedStart;
		/*Enable interrupt when ever RXNE flag is set*/
		I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CR2_ITBUFEN;
		I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CR2_ITEVTEN;
		I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CR2_ITERREN;
		/*Enable ACKing*/
		I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr,ENABLE);
		/*Generate start condition*/
		I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,START);
	}
	return state;
}

/***********************************************************************
Master send data (interrupt base)
***********************************************************************/
uint8_t I2C_master_send_intrpt (I2C_Handle_t *I2CxHandlePtr, uint8_t *txBufferPtr, uint32_t Length,uint8_t slaveAddr,uint8_t repeatedStart)
{
		uint8_t state = I2CxHandlePtr->State;
		if(I2CxHandlePtr->State == I2C_READY){
		I2CxHandlePtr->txBufferPtr = txBufferPtr;
		I2CxHandlePtr->txLength = Length;
		I2CxHandlePtr->State = I2C_BUSY_IN_TX;
		I2CxHandlePtr->slaveAddr = slaveAddr;
		I2CxHandlePtr->repeatedStart = repeatedStart;	
		/*Enable interrupt when TXE flag is set or when error occur*/
		I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CR2_ITBUFEN;
		I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CR2_ITEVTEN;
		I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CR2_ITERREN;
		/*Generate start condition*/
		I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,START);
	}
	return state;
}

/***********************************************************************
Slave receive data
***********************************************************************/
uint8_t I2C_slave_receive (I2C_TypeDef *I2CxPtr)
{
	return (uint8_t) I2CxPtr->DR;
}

/***********************************************************************
Slave send data
***********************************************************************/
void I2C_slave_send(I2C_TypeDef *I2CxPtr, uint8_t dataByte)
{
	I2CxPtr->DR = dataByte;
}

/***********************************************************************
Enable or disable I2C 's interrupt 
***********************************************************************/
void I2C_intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis)
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
Config interrupt priority for I2C peripheral 
***********************************************************************/
void I2C_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	NVIC->IP[registerNo] &= ~(0xFF << (8*section));
	NVIC->IP[registerNo] |= (priority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}

/***********************************************************************
I2C error interrupt handler 
***********************************************************************/
void I2C_err_intrpt_handler (I2C_Handle_t *I2CxHandlePtr)
{
	/*case interrupt triggered by bus error*/
	if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_BERR){
		I2CxHandlePtr->I2CxPtr->SR1 &= ~(I2C_SR1_BERR);
		I2C_application_event_callback(I2CxHandlePtr,I2C_ERR_BERR);
	
	/*case interrupt triggered by arbitration lost error*/
	}else if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_ARLO){
		I2CxHandlePtr->I2CxPtr->SR1 &= ~(I2C_SR1_ARLO);
		I2C_application_event_callback(I2CxHandlePtr,I2C_ERR_ARLO);
		
	/*case interrupt triggered by acknowledge failure error*/	
	}else if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_AF){
		I2CxHandlePtr->I2CxPtr->SR1 &= ~(I2C_SR1_AF);
		I2C_application_event_callback(I2CxHandlePtr,I2C_ERR_AF);
		
	/*case interrupt triggered by overun error*/	
	}else if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_OVR){
		I2CxHandlePtr->I2CxPtr->SR1 &= ~(I2C_SR1_OVR);
		I2C_application_event_callback(I2CxHandlePtr,I2C_ERR_OVR);
		
	/*case interrupt triggered by timeout error*/
	}else if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_TIMEOUT){
		I2CxHandlePtr->I2CxPtr->SR1 &= ~(I2C_SR1_TIMEOUT);
		I2C_application_event_callback(I2CxHandlePtr,I2C_ERR_TIMEOUT);
	}
}

/***********************************************************************
I2C event interrupt handler 
@Note: common interrupt handler for master and slave
***********************************************************************/
void I2C_event_intrpt_handler (I2C_Handle_t *I2CxHandlePtr)
{
	
	/*case interrupt is triggered by SB flag (start condition generation is detected)*/
	/*this block is only executed in master mode*/
	if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_SB){
		
		if(I2CxHandlePtr->State == I2C_BUSY_IN_RX){
			I2C_address_phase_execute(I2CxHandlePtr->I2CxPtr,I2CxHandlePtr->slaveAddr,READ);
		}else if(I2CxHandlePtr->State == I2C_BUSY_IN_TX){
			I2C_address_phase_execute(I2CxHandlePtr->I2CxPtr,I2CxHandlePtr->slaveAddr,WRITE);
		}
	}
	
	/*case interrupt is triggered by ADDR flag (slave address sent (for master) or slave address matched (for slave))*/
	if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_ADDR){
		I2C_clear_ADDRflag(I2CxHandlePtr);
	}
	
	/*case interrupt is triggered by BTF flag*/
	/**@note add case when device is in slave mode*/
	if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_BTF){
		
		/*when in data transmission, length = 0 & BTF = 1 & TXE = 1 indicate transmission is completed, therefore generate stop condition (if repeated start is disable)*/
		if(I2CxHandlePtr->State == I2C_BUSY_IN_TX){
			if(!I2CxHandlePtr->txLength){
				if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_TXE){
					if(I2CxHandlePtr->repeatedStart == I2C_REPEATED_START_DISABLE){
						I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr, STOP);
					}
				}
			}
		}
		I2C_close_send_data(I2CxHandlePtr);
		I2C_application_event_callback(I2CxHandlePtr,I2C_EV_MST_TX_CMPLT);
	}
		
	/*case interrupt is triggered by STOFF flag (slave detected stop condition)*/
	/*this block is only executed in slave mode*/
	if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_STOPF){
		I2C_clear_STOPFflag(I2CxHandlePtr->I2CxPtr);
		I2C_application_event_callback(I2CxHandlePtr,I2C_EV_SLV_STOP_DETECTED);
	}
	
	/*case interrupt is triggered by RXNE flag*/
	if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_RXNE){
		
		/*case device is in master mode*/
		if(I2CxHandlePtr->I2CxPtr->SR2 & I2C_SR2_MSL){	
			
			/*case slave only send 1 byte of data*/
			if(I2CxHandlePtr->rxLength == 1){
				
				I2C_read_data(I2CxHandlePtr->I2CxPtr, I2CxHandlePtr->rxBufferPtr, &(I2CxHandlePtr->rxLength));
				I2CxHandlePtr->rxBufferPtr++;
			
			/*case slave send multiple data bytes*/
			}else if (I2CxHandlePtr->rxLength > 1){
				
				if(I2CxHandlePtr->rxLength == 2){	
					I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
				}	
				I2C_read_data(I2CxHandlePtr->I2CxPtr, I2CxHandlePtr->rxBufferPtr, &(I2CxHandlePtr->rxLength));
				I2CxHandlePtr->rxBufferPtr++;
			}
			
			/*after all bytes were received*/
			if (I2CxHandlePtr->rxLength == 0){				
				if(I2CxHandlePtr->repeatedStart == I2C_REPEATED_START_DISABLE){
					I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr, STOP);
				}
				I2C_close_receive_data(I2CxHandlePtr);
				I2C_application_event_callback(I2CxHandlePtr,I2C_EV_MST_RX_CMPLT);
			}
		
		/*case device is in slave mode*/
		}else{
			if(!(I2CxHandlePtr->I2CxPtr->SR2 & I2C_SR2_TRA)){
				I2C_application_event_callback(I2CxHandlePtr,I2C_EV_SLV_READ);
			}
		}
	}
	
	/*case interrupt is triggered by TXE flag*/
	if(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_TXE){
		
		/*case device is in master mode*/
		if(I2CxHandlePtr->I2CxPtr->SR2 & I2C_SR2_MSL){
			if(I2CxHandlePtr->txLength){
				I2C_send_data(I2CxHandlePtr->I2CxPtr,I2CxHandlePtr->txBufferPtr,&(I2CxHandlePtr->txLength));
				I2CxHandlePtr->txBufferPtr++;
			}
		
		/*case device is in slave mode*/
		}else{
			if(I2CxHandlePtr->I2CxPtr->SR2 & I2C_SR2_TRA){
				I2C_application_event_callback(I2CxHandlePtr,I2C_EV_SLV_DT_REQ);
			}
		}
	}
}

/***********************************************************************
inform application of I2C event or error
@Note: this is to be define in user application
***********************************************************************/
__attribute__((weak)) void I2C_application_event_callback (I2C_Handle_t *I2CxHandlePtr,uint8_t event) 
{
}
