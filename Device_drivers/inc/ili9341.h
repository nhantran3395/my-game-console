/**
*@file ili9341.h
*@brief provide functions for interfacing with ILI9341.LCD controller.
*
*This header file provide functions for interfacing with ILI9341 LCD controller.
*The functions in this library is mainly based on Tilen Majerle work.  I just modified so that these functions  will use my SPI driver library functions instead.
*
*@author Tran Thanh Nhan
*@date 03/09/2019
*
*@reference	https://stm32f4-discovery.net/2014/04/library-08-ili9341-lcd-on-stm32f429-discovery-board/
*/

/**
*@Version 1.0 
*03/09/2019
*/

#ifndef ILI9341_H
#define ILI9341_H

#include "stm32f407xx.h"                  // Device header
#include "../../Peripheral_drivers/inc/stm32f407xx_common_macro.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../../Peripheral_drivers/inc/stm32f407xx_spi.h"
#include "../../Miscellaneous/inc/tm_stm32f4_fonts.h"
#include <stdint.h>
#include <stdlib.h>

/***********************************************************************
ILI9341 macro definition
***********************************************************************/

/*
*@ILI9341_SPI
*ILI9341 SPI peripheral and pins pack selection
*/
#define ILI9341_SPI	SPI1
#define ILI9341_SPI_PINS_PACK SPI_pins_pack_2

/*
*@ILI9341_CSX
*ILI9341 chip enable input GPIO port & pin selection
*/
#define ILI9341_CSX_PORT  GPIOD
#define ILI9341_CSX_PIN	GPIO_PIN_NO_6

/*
*@ILI9341_DCX
*ILI9341 data/command input GPIO port & pin selection 
*/
#define ILI9341_DCX_PORT  GPIOD
#define ILI9341_DCX_PIN	GPIO_PIN_NO_4

/*
*@ILI9341_RST
*ILI9341 reset GPIO port & pin selection 
*/
#define ILI9341_RST_PORT  GPIOD
#define ILI9341_RST_PIN	GPIO_PIN_NO_2

/*
*@ILI9341_COMMAND
*ILI9341 command
*/
#define ILI9341_RESET													0x01
#define ILI9341_SLEEP_OUT									0x11
#define ILI9341_GAMMA												0x26
#define ILI9341_DISPLAY_OFF								0x28
#define ILI9341_DISPLAY_ON								0x29
#define ILI9341_COLUMN_ADDR						0x2A
#define ILI9341_PAGE_ADDR									0x2B
#define ILI9341_MEM_WRITE									0x2C
#define ILI9341_MAC															0x36
#define ILI9341_PIXEL_FORMAT							0x3A
#define ILI9341_WDB															0x51
#define ILI9341_WCD															0x53
#define ILI9341_RGB_INTERFACE					0xB0
#define ILI9341_FRC															0xB1
#define ILI9341_BPC															0xB5
#define ILI9341_DFC															0xB6
#define ILI9341_POWER1											0xC0
#define ILI9341_POWER2											0xC1
#define ILI9341_VCOM1													0xC5
#define ILI9341_VCOM2													0xC7
#define ILI9341_POWERA											0xCB
#define ILI9341_POWERB											0xCF
#define ILI9341_PGAMMA											0xE0
#define ILI9341_NGAMMA											0xE1
#define ILI9341_DTCA														0xE8
#define ILI9341_DTCB														0xEA
#define ILI9341_POWER_SEQ								0xED
#define ILI9341_3GAMMA_EN								0xF2
#define ILI9341_INTERFACE										0xF6
#define ILI9341_PRC															0xF7

/* 
*@ILI9341_COLOR
*Color code
*/
#define ILI9341_BLACK								0x0000 
#define ILI9341_NAVY									0x000F 
#define ILI9341_DARKGREEN   		0x03E0 
#define ILI9341_DARKCYAN    			0x03EF 
#define ILI9341_MAROON      			0x7800  
#define ILI9341_PURPLE      				0x780F  
#define ILI9341_OLIVE       					0x7BE0  
#define ILI9341_LIGHTGREY   			0xC618  
#define ILI9341_DARKGREY    		0x7BEF 
#define ILI9341_BLUE        					0x001F  
#define ILI9341_GREEN       				0x07E0  
#define ILI9341_CYAN        					0x07FF  
#define ILI9341_RED         						0xF800  
#define ILI9341_MAGENTA     			0xF81F  
#define ILI9341_YELLOW      				0xFFE0  
#define ILI9341_WHITE       					0xFFFF  
#define ILI9341_ORANGE      				0xFD20  
#define ILI9341_GREENYELLOW	0xAFE5  
#define ILI9341_PINK										0xFC18  

#define ILI9341_WIDTH		240
#define ILI9341_HEIGHT		320
#define ILI9341_PIXEL			76800

#define ILI9341_CSX_SET			GPIO_write_pin(ILI9341_CSX_PORT,ILI9341_CSX_PIN,SET)
#define ILI9341_CSX_CLEAR 	GPIO_write_pin(ILI9341_CSX_PORT,ILI9341_CSX_PIN,CLEAR)
#define ILI9341_DCX_SET 			GPIO_write_pin(ILI9341_DCX_PORT,ILI9341_DCX_PIN,SET)
#define ILI9341_DCX_CLEAR 	GPIO_write_pin(ILI9341_DCX_PORT,ILI9341_DCX_PIN,CLEAR)
#define ILI9341_RST_SET 			GPIO_write_pin(ILI9341_RST_PORT,ILI9341_RST_PIN,SET)
#define ILI9341_RST_CLEAR 	GPIO_write_pin(ILI9341_RST_PORT,ILI9341_RST_PIN,CLEAR)

/***********************************************************************
ILI9341 structure and enumeration definition
***********************************************************************/
typedef enum {
	ILI9341_orientation_portrait_1,
	ILI9341_orientation_portrait_2,
	ILI9341_orientation_landscape_1,
	ILI9341_orientation_landscape_2,
}ILI9341_Orientation_e;

typedef struct{
	uint16_t width;
	uint16_t height;
	ILI9341_Orientation_e orientation;
}ILI9341_Config_t;
/***********************************************************************
ILII9341 driver function prototype
***********************************************************************/

/**
*@brief  		Initilaize related hardware (GPIO pins, SPI peripheral and initilize display with default settings
*@param 	None
*@return 	None
*/
void ILI9341_init (void);

/**
*@brief 		Turn on display
*@param 	None
*@return		None 
*/
void ILI9341_display_on (void);

/**
*@brief 		Turn of display
*@param 	None
*@return 	None
*/
void ILI9341_display_off (void);

/**
*@brief 		Draw pixel in specified (x,y) position
*@param 	X axis value
*@param 	Y axis value 
*@param 	Color
*@return  	None
*/
void ILI9341_draw_pixel (int16_t x,int16_t y, uint16_t color);

/**
*@brief 			Draw monochrome image starting from specified (x,y) position
*@param	X axis value of top left conner pixel of image
*@param	Y axis value of top left conner pixel of image
*@param 	Byte array with monochrome bitmap
*@param 	Color
*@return 	None
*/
void ILI9341_draw_bitmap (int16_t x, int16_t y, const uint8_t *bitmapPtr, uint16_t w, uint16_t h, uint16_t color);

/**
*@brief 			Draw monochrome image starting from specified (x,y) position (with background)
*@param	X axis value of top left conner pixel of image
*@param	Y axis value of top left conner pixel of image
*@param 	Byte array with monochrome bitmap
*@param 	Foreground color
*@param 	Background color
*@return 	None
*/
void ILI9341_draw_bitmap_w_background (int16_t x, int16_t y, const uint8_t *bitmapPtr, uint16_t w, uint16_t h, uint16_t foreground, uint16_t background);

/**
*@brief 			Draw 16-bits color image starting from specified (x,y) position
*@param	X axis value of top left conner pixel of BMP 
*@param	Y axis value of top left conner pixel of BMP
*@param 	2-byte array with 16-bits color bitmap
*@return 	None
*/
void ILI9341_draw_RGB_bitmap (int16_t x, int16_t y, const uint16_t bitmap[], uint16_t w, uint16_t h);

/**
*@brief 		Rotate LCD in specified orientation
*@param	Orientation 	
*@return 	None
*/
void ILI9341_rotate (ILI9341_Orientation_e orientation);

/**
*@brief 		Fill entire display with specified color
*@param 	Color
*@return 	None
*/
void ILI9341_fill_display (uint16_t color);

/**
*@brief 			Put character starting from (x,y) position
*@param 	X axis value of top left corner pixel of character
*@param 	Y axis value of top left corner pixel of character
*@param 	Character
*@param 	Pointer to font structure
*@param 	Foreground color
*@return 	None
*/
void ILI9341_put_character (uint16_t x, uint16_t y, char c, TM_FontDef_t *font, uint32_t foreground);

/**
*@brief 			Put character starting from (x,y) position
*@param 	X axis value of top left corner pixel of character
*@param 	Y axis value of top left corner pixel of character
*@param 	Character
*@param 	Pointer to font structure
*@param 	Foreground color
*@param	Background color
*@return 	None
*/
void ILI9341_put_character_w_background (uint16_t x, uint16_t y, char c, TM_FontDef_t *font, uint32_t foreground, uint32_t background);

/**
*@brief 		Put character starting from (x,y) position
*@param 	X axis value of top left corner pixel of character
*@param 	Y axis value of top left corner pixel of character
*@param 	Pointer to string 
*@param 	Pointer to font structure
*@param 	Foreground color
*@param	Background color
*@return 	None
*/
void ILI9341_put_string (uint16_t x, uint16_t y, char *str, TM_FontDef_t *font, uint32_t foreground);

/**
*@brief 		Put character starting from (x,y) position ( no background)
*@param 	X axis value of top left corner pixel of character
*@param 	Y axis value of top left corner pixel of character
*@param 	Pointer to string 
*@param 	Pointer to font structure
*@param 	Foreground color
*@return 	None
*/
void ILI9341_put_string_w_background (uint16_t x, uint16_t y, char *str, TM_FontDef_t *font, uint32_t foreground, uint32_t background);

/**
*@brief 		Draw line starting from (x0,y0) to (x1,y)
*@param 	X axis value of starting point
*@param 	Y axis value of starting point
*@param 	X axis value of ending point
*@param 	Y axis value of ending point
*@param 	Color
*@return 	None
*/
void ILI9341_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);

/**
*@brief 		Draw rectangle which top left point is (x0,y0) and bottom right point is (x1,y1)
*@param 	X axis value of top left point
*@param 	Y axis value of top left point
*@param 	X axis value of bottom right point
*@param 	Y axis value of bottom right point
*@param 	Color
*@return 	None
*/
void ILI9341_draw_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);

/**
*@brief 		Draw filled rectangle which top left point is (x0,y0) and bottom right point is (x1,y1)
*@param 	X axis value of top left point
*@param 	Y axis value of top left point
*@param 	X axis value of bottom right point
*@param 	Y axis value of bottom right point
*@param 	Color
*@return 	None
*/
void ILI9341_draw_filled_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color) ;

/**
*@brief 		Draw circle which center is (x0,y0) 
*@param 	X axis value of center
*@param 	Y axis value of center
*@param 	Radius
*@param 	Color
*@return 	None
*/
void ILI9341_draw_circle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/**
*@brief 		Draw filled circle which center is (x0,y0) 
*@param 	X axis value of center
*@param 	Y axis value of center
*@param 	Radius
*@param 	Color
*@return 	None
*/
void ILI9341_draw_filled_circle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
#endif 
