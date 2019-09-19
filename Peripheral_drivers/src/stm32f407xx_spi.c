/**
*@file stm32f407xx_spi.c
*@brief provide functions for interfacing with SPI peripherals.
*
*This implementation file provide functions for communication between stm32f407xx MCU and I/O devices through SPI protocol.
*
*@author Tran Thanh Nhan
*@date 28/07/2019
*
*@reference	https://stm32f4-discovery.net/2014/04/library-05-spi-for-stm32f4xx/
*@reference	Mastering microcontroller with embedded driver development course on Udemy		
*/

#include "../inc/stm32f407xx_spi.h"

static void SPI_pins_pack_1_GPIO_init(SPI_TypeDef *SPIxPtr);
static void SPI_pins_pack_2_GPIO_init(SPI_TypeDef *SPIxPtr);
static void SPI_pins_pack_3_GPIO_init(SPI_TypeDef *SPIxPtr);
static void SPI_data_frame_config(SPI_TypeDef *SPIxPtr, uint8_t dataFrame);
static void SPI_close_transmission(SPI_Handle_t *SPIxHandlePtr);
static void SPI_close_reception(SPI_Handle_t *SPIxHandlePtr);


/***********************************************************************
SPI clock enable/disable
***********************************************************************/
void SPI_CLK_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE)
	{
		if(SPIxPtr == SPI1){
			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
		}else if(SPIxPtr == SPI2){
			RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
		}else if(SPIxPtr == SPI3){
			RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	}	
	}else{
		if(SPIxPtr == SPI1){
			RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN);
		}else if(SPIxPtr == SPI2){
			RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN);
		}else if(SPIxPtr == SPI3){
			RCC->APB1ENR &= ~(RCC_APB1ENR_SPI3EN);
		}	
	}
}

/***********************************************************************
SPI peripheral enable/disable
***********************************************************************/
void SPI_periph_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		SPIxPtr->CR1 |= SPI_CR1_SPE;
	}else{
		SPIxPtr->CR1 &= ~(SPI_CR1_SPE);
	}
}

/***********************************************************************
Enable use of NSS pin in Hardware Slave Management (SSM = 0) mode, in case MCU is used as Master.
***********************************************************************/
void SPI_NSS_pin_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		SPIxPtr->CR2 |= SPI_CR2_SSOE;
	}else{
		SPIxPtr->CR2 &= ~(SPI_CR2_SSOE);
	}
}

/***********************************************************************

***********************************************************************/
void SPI_SSI_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		SPIxPtr->CR1 |= SPI_CR1_SSI;
	}else{
		SPIxPtr->CR1 &= ~SPI_CR1_SSI;
	}
}

/***********************************************************************
Check whether SPI is busy in communication or not 
***********************************************************************/
uint8_t SPI_busy_check(SPI_TypeDef *SPIxPtr)
{
		return (SPIxPtr->SR >> SPI_SR_BSY_Pos) & 0x01; 
}


/***********************************************************************
Initialize SPI peripheral
***********************************************************************/
void SPI_init(SPI_Handle_t *SPIxHandlePtr)
{
	SPI_CLK_ctr(SPIxHandlePtr->SPIxPtr,ENABLE);
	
	SPI_periph_ctr(SPIxHandlePtr->SPIxPtr,DISABLE);
	
	/*device mode: master or slave*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_MSTR);
	SPIxHandlePtr->SPIxPtr->CR1 |= (SPIxHandlePtr->SPIxConfigPtr->deviceMode << SPI_CR1_MSTR_Pos);
	
	if(SPIxHandlePtr->SPIxConfigPtr->busConfig == SPI_BUS_HALF_DUPLEX){
		/*configuration for half duplex*/
		SPIxHandlePtr->SPIxPtr->CR1 |= SPI_CR1_BIDIMODE;
	}else{
		/*general configuration for full duplex and simplex receive only*/
		SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_BIDIMODE);
		
		if(SPIxHandlePtr->SPIxConfigPtr->busConfig == SPI_BUS_FULL_DUPLEX){
			/*full duplex*/
			SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_RXONLY);
		}else{
			/*simplex receive only*/
			SPIxHandlePtr->SPIxPtr->CR1 |= SPI_CR1_RXONLY;
		}
	}
	
	/*data frame: 8 bits or 16 bits*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_DFF);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->dataFrame << SPI_CR1_DFF_Pos;

	/*clock phase: 1st edge or 2nd edge*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_CPHA);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->clkPhase << SPI_CR1_CPHA_Pos;	
	
	/*clock polarity: high idle or low idle*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_CPOL);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->clkPol << SPI_CR1_CPOL_Pos;
	
	/*configure software slave management*/
	uint8_t option = SPIxHandlePtr->SPIxConfigPtr->swSlaveManage;
	SPIxHandlePtr->SPIxPtr->CR1 &= ~SPI_CR1_SSM;
	SPIxHandlePtr->SPIxPtr->CR1 |= option <<SPI_CR1_SSM_Pos;
	
	/*clock speed: f_pCLK/2 -> f_pCLK/256*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_BR);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->clkSpeed << SPI_CR1_BR_Pos;	

}

/***********************************************************************
Initialize SPI peripheral and initialize corresponding GPIO pins as SPI function
***********************************************************************/
SPI_Handle_t* SPI_general_init(SPI_TypeDef *SPIxPtr, SPI_pins_pack_t pinsPack, uint32_t deviceMode, uint8_t busConfig, uint8_t dataFrame, uint8_t clkPhase, uint8_t clkPol, uint8_t swSlaveManage, uint8_t clkSpeed)
{
	if (pinsPack == SPI_pins_pack_1){
		SPI_pins_pack_1_GPIO_init(SPIxPtr);
	}else if (pinsPack == SPI_pins_pack_2){
		SPI_pins_pack_2_GPIO_init(SPIxPtr);
	}else if (pinsPack == SPI_pins_pack_3){
		SPI_pins_pack_3_GPIO_init(SPIxPtr);		
	}
	
	static SPI_Handle_t SPIxHandle;
	static SPI_Config_t SPIxConfig;
	
	SPIxConfig.deviceMode = deviceMode;
	SPIxConfig.busConfig = busConfig;
	SPIxConfig.dataFrame = dataFrame;
	SPIxConfig.clkPhase = clkPhase;
	SPIxConfig.clkPol = clkPol;
	SPIxConfig.swSlaveManage = swSlaveManage;
	SPIxConfig.clkSpeed = clkSpeed;
	
	SPIxHandle.SPIxPtr = SPIxPtr;
	SPIxHandle.SPIxConfigPtr = &SPIxConfig;
	SPI_init(&SPIxHandle);
	
	return &SPIxHandle;
}

/***********************************************************************
Deinitialize SPI peripheral
***********************************************************************/
void SPI_deinit(SPI_TypeDef *SPIxPtr)
{
	if(SPIxPtr == SPI1){
		RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
		RCC->AHB2RSTR &= ~(RCC_APB2RSTR_SPI1RST);
	}else if(SPIxPtr == SPI2){
		RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_SPI2RST);
	}else if(SPIxPtr == SPI3){
		RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_SPI3RST);
	}
}


/***********************************************************************
Receive multiple bytes from SPI 
***********************************************************************/
void SPI_receive_data (SPI_TypeDef *SPIxPtr, uint8_t *rxBufferPtr,uint32_t Length)
{
			while(Length){
			/* wait until rx buffer is not empty*/
			while(!(SPIxPtr->SR & SPI_SR_RXNE));
			
			if(!(SPIxPtr->CR1 & SPI_CR1_DFF)){
				/*8 bit data frame*/
				*rxBufferPtr = SPIxPtr->DR;
				rxBufferPtr++;
				Length--;
			}else{
				/*16 bit data frame*/
				*((uint16_t*)rxBufferPtr) = SPIxPtr->DR ;
				rxBufferPtr+=2;
				Length-=2;
			}
		}
}

/***********************************************************************
Send multiple bytes through SPI
***********************************************************************/
void SPI_send_data (SPI_TypeDef *SPIxPtr, uint8_t *txBufferPtr, uint32_t Length)
{
		while(Length){
			/* wait until tx buffer is empty*/
			while(!(SPIxPtr->SR & SPI_SR_TXE));
			
			if(!(SPIxPtr->CR1 & SPI_CR1_DFF)){
				/*8 bit data frame*/
				SPIxPtr->DR = *txBufferPtr;
				txBufferPtr++;
				Length--;
			}else{
				/*16 bit data frame*/
				SPIxPtr->DR = *((uint16_t*)txBufferPtr);
				txBufferPtr+=2;
				Length-=2;
			}
		}
}

/***********************************************************************
Send 1 byte of data through SPI using 8 bits data frame configuration 
***********************************************************************/
void SPI_send_8_bits(SPI_TypeDef *SPIxPtr, uint8_t data)
{
	SPI_data_frame_config(SPIxPtr,SPI_DATA_8BITS);
	
	/* wait until tx buffer is empty*/
	while(!(SPIxPtr->SR & SPI_SR_TXE));
	SPIxPtr->DR = data;
}

/***********************************************************************
Send 2 byte (16 bits) of data at a time through SPI using 16 bits data frame configuration 
***********************************************************************/
void SPI_send_16_bits(SPI_TypeDef *SPIxPtr, uint16_t data)
{
	SPI_data_frame_config(SPIxPtr,SPI_DATA_16BITS);
	
	/* wait until tx buffer is empty*/
	while(!(SPIxPtr->SR & SPI_SR_TXE));
	SPIxPtr->DR = data;
}
	
/***********************************************************************
Receive multiple bytes from SPI (interrup base) 
***********************************************************************/
uint8_t SPI_receive_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length)
{
	uint8_t state = SPIxHandlePtr->rxState;
	if(SPIxHandlePtr->rxState == SPI_STATE_READY){
		SPIxHandlePtr->rxBufferPtr = rxBufferPtr;
		SPIxHandlePtr->rxLength = Length;
		SPIxHandlePtr->rxState = SPI_STATE_RX_BUSY;
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_ERRIE;
		/*Enable RXNEIE bit to get interrupt when ever RXNE flag is set*/
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_RXNEIE;
	}
	return state;
}

/***********************************************************************
Send multiple bytes to SPI (interrupt base)
***********************************************************************/
uint8_t SPI_send_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *txBufferPtr, uint32_t Length)
{
	uint8_t state = SPIxHandlePtr->txState;
	if(SPIxHandlePtr->txState == SPI_STATE_READY){
		SPIxHandlePtr->txBufferPtr = txBufferPtr;
		SPIxHandlePtr->txLength = Length;
		SPIxHandlePtr->txState = SPI_STATE_TX_BUSY;
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_ERRIE;
		/*Enable TXEIE bit to get interrupt when ever TXE flag is set*/
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_TXEIE;
	}
	return state;
}

/***********************************************************************
Enable or disable SPI 's interrupt request in NVIC
***********************************************************************/
void SPI_intrpt_vector_ctrl (uint8_t IRQnumber, uint8_t enOrDis)
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
	Config interrupt priority for SPI peripheral
***********************************************************************/
void SPI_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	NVIC->IP[registerNo] &= ~(0xFF << (8*section));
	NVIC->IP[registerNo] |= (priority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}


/***********************************************************************
General Interrupt handler for SPI peripheral
***********************************************************************/
void SPI_intrpt_handler (SPI_Handle_t *SPIxHandlePtr)
{
	/*case interrupt triggered due to data received in rx buffer*/
	uint8_t check1 = (SPIxHandlePtr->SPIxPtr->CR2 & SPI_CR2_RXNEIE) >> SPI_CR2_RXNEIE_Pos;
	uint8_t check2 = (SPIxHandlePtr->SPIxPtr->SR  & SPI_SR_RXNE) >> SPI_SR_RXNE_Pos;
	
	if(check1 & check2){
		if(SPIxHandlePtr->SPIxConfigPtr->dataFrame == SPI_DATA_8BITS){
			*(SPIxHandlePtr->rxBufferPtr) = SPIxHandlePtr->SPIxPtr->DR;
			SPIxHandlePtr->rxLength--;
			SPIxHandlePtr->rxBufferPtr++;
		}else{
			*((uint16_t*)SPIxHandlePtr->rxBufferPtr) = SPIxHandlePtr->SPIxPtr->DR;
			SPIxHandlePtr->rxLength-=2;
			SPIxHandlePtr->rxBufferPtr+=2;
		}
		if(!(SPIxHandlePtr->rxLength)){
			SPIxHandlePtr->rxState = SPI_STATE_READY;
			SPIxHandlePtr->SPIxPtr->CR2 &= ~(SPI_CR2_RXNEIE);
		}
	}
	
	/*case interrupt triggered due to tx buffer is empty*/
	check1 = (SPIxHandlePtr->SPIxPtr->CR2 & SPI_CR2_TXEIE) >> SPI_CR2_TXEIE_Pos;
	check2 = (SPIxHandlePtr->SPIxPtr->SR & SPI_SR_TXE) >> SPI_SR_TXE_Pos;
	
	if(check1 & check2){
		
		if(SPIxHandlePtr->txLength){
			if(SPIxHandlePtr->SPIxConfigPtr->dataFrame == SPI_DATA_8BITS){
				SPIxHandlePtr->SPIxPtr->DR = *(SPIxHandlePtr->txBufferPtr);
				SPIxHandlePtr->txLength--;
				SPIxHandlePtr->txBufferPtr++;
			}else{
				SPIxHandlePtr->SPIxPtr->DR = *((uint16_t*)SPIxHandlePtr->rxBufferPtr);
				SPIxHandlePtr->txLength-=2;
				SPIxHandlePtr->txBufferPtr+=2;
			}
		}
		
		if(!(SPIxHandlePtr->txLength)){
			
			SPI_close_transmission(SPIxHandlePtr);
			SPI_application_event_callback(SPIxHandlePtr,SPI_EV_TRANSMISSION_CMPLT);
			
			SPIxHandlePtr->txState = SPI_STATE_READY;
			SPIxHandlePtr->SPIxPtr->CR2 &= ~(SPI_CR2_TXEIE);
		}
	}
	
	/*case interrupt triggered due to overrun error*/
	check1 = (SPIxHandlePtr->SPIxPtr->CR2 & SPI_CR2_ERRIE) >> SPI_CR2_ERRIE_Pos;
	check2 = (SPIxHandlePtr->SPIxPtr->SR & SPI_SR_OVR) >> SPI_SR_OVR_Pos;
	
	if(check1 & check2){
		uint8_t temp;
		temp = SPIxHandlePtr->SPIxPtr->DR;
		temp = SPIxHandlePtr->SPIxPtr->SR;
		(void) temp;
	}
}

/***********************************************************************
Inform application of SPI event or error
@Note: this is to be define in user application
***********************************************************************/
__attribute__((weak)) void SPI_application_event_callback (SPI_Handle_t *SPIxHandlePtr,uint8_t event) 
{
}

/***********************************************************************
Private function: Initialize GPIO pin as SPI function (for pins pack 1) 
***********************************************************************/
void SPI_pins_pack_1_GPIO_init(SPI_TypeDef *SPIxPtr)
{
	if(SPIxPtr == SPI1){
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_5,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_6,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOA,GPIO_PIN_NO_7,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
	}else if(SPIxPtr == SPI2){
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_10,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_2,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_3,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);			
	}else if(SPIxPtr == SPI3){
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_3,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,6);
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_4,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,6);
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_5,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,6);
	}		
}

/***********************************************************************
Private function: Initialize GPIO pin as SPI function (for pins pack 2) 
***********************************************************************/
void SPI_pins_pack_2_GPIO_init(SPI_TypeDef *SPIxPtr)
{
	if(SPIxPtr == SPI1){
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_3,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_4,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_5,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);		
	}else if(SPIxPtr == SPI2){
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_13,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_14,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOB,GPIO_PIN_NO_15,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
	}else if(SPIxPtr == SPI3){
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_10,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,6);
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_11,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,6);
		GPIO_init_direct(GPIOC,GPIO_PIN_NO_12,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,6);
	}			
}

/***********************************************************************
Private function: Initialize GPIO pin as SPI function (for pins pack 3) 
***********************************************************************/
void SPI_pins_pack_3_GPIO_init(SPI_TypeDef *SPIxPtr)
{
	if(SPIxPtr == SPI2){
		GPIO_init_direct(GPIOI,GPIO_PIN_NO_1,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOI,GPIO_PIN_NO_2,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);
		GPIO_init_direct(GPIOI,GPIO_PIN_NO_3,GPIO_MODE_ALTFN,GPIO_OUTPUT_VERY_HIGH_SPEED,GPIO_OUTPUT_TYPE_PP,GPIO_NO_PUPDR,5);				
	}
}

/***********************************************************************
Private function: Configure data frame format at run time
***********************************************************************/
void SPI_data_frame_config(SPI_TypeDef *SPIxPtr, uint8_t dataFrame)
{			
		SPI_periph_ctr(SPIxPtr,DISABLE);
		
		if(dataFrame == SPI_DATA_8BITS){
			SPIxPtr->CR1 &= ~SPI_CR1_DFF;
		}else if (dataFrame == SPI_DATA_16BITS){
			SPIxPtr->CR1 |= SPI_CR1_DFF;
		}
	
		SPI_periph_ctr(SPIxPtr,ENABLE);
}

/***********************************************************************
Private function: Close SPI transmission (after transmission complete) 
***********************************************************************/
void SPI_close_transmission(SPI_Handle_t *SPIxHandlePtr)
{
	SPIxHandlePtr->txBufferPtr = NULL;
	SPIxHandlePtr->txLength = 0;
	SPIxHandlePtr->txState = SPI_STATE_READY;
}

/***********************************************************************
Private function: Close SPI reception (after reception complete) 
***********************************************************************/
void SPI_close_reception(SPI_Handle_t *SPIxHandlePtr)
{
	SPIxHandlePtr->rxBufferPtr = NULL;
	SPIxHandlePtr->rxLength = 0;
	SPIxHandlePtr->rxState = SPI_STATE_READY;
}