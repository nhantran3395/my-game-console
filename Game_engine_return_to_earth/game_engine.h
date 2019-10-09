/**
*@file game_engine.h
*@brief "Return To Earth" game engine library.
*
*This header file provide functions for making handheld console game "Return To Earth".
*
*@author Tran Thanh Nhan
*@date 09/09/2019
*/

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../Peripheral_drivers/inc/stm32f407xx_timer.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rcc.h"
#include "../Peripheral_drivers/inc/stm32f407xx_rng.h"
#include "../Device_drivers/inc/ili9341.h"
#include "../Device_drivers/inc/joystick.h"
#include "../Device_drivers/inc/button.h"
#include "../Device_drivers/inc/led.h"
#include "../Device_drivers/inc/speaker.h"
#include "vector.h"
#include <math.h>
#include <stdio.h>

/***********************************************************************
Macro definition
***********************************************************************/
#define SHOOT_BUTTON_PORT		GPIOC
#define	SHOOT_BUTTON_PIN		GPIO_PIN_NO_1
#define	SHOOT_BUTTON_READ		button_read(SHOOT_BUTTON_PORT,SHOOT_BUTTON_PIN)

#define THRUST_BUTTON_PORT		GPIOC
#define	THRUST_BUTTON_PIN		GPIO_PIN_NO_3
#define	THRUST_BUTTON_READ		button_read(THRUST_BUTTON_PORT,THRUST_BUTTON_PIN)

#define PROTOBOARD_RED_LED_PORT		GPIOC
#define PROTOBOARD_RED_LED_PIN		GPIO_PIN_NO_0
#define PROTOBOARD_RED_LED_ON		led_on(PROTOBOARD_RED_LED_PORT,PROTOBOARD_RED_LED_PIN)
#define PROTOBOARD_RED_LED_OFF		led_off(PROTOBOARD_RED_LED_PORT,PROTOBOARD_RED_LED_PIN)

#define PROTOBOARD_GREEN_LED_PORT	GPIOC
#define PROTOBOARD_GREEN_LED_PIN	GPIO_PIN_NO_2
#define PROTOBOARD_GREEN_LED_ON		led_on(PROTOBOARD_GREEN_LED_PORT,PROTOBOARD_GREEN_LED_PIN)
#define PROTOBOARD_GREEN_LED_OFF	led_off(PROTOBOARD_GREEN_LED_PORT,PROTOBOARD_GREEN_LED_PIN)

#define PROTOBOARD_BLUE_LED_PORT	GPIOA
#define PROTOBOARD_BLUE_LED_PIN		GPIO_PIN_NO_0
#define PROTOBOARD_BLUE_LED_ON		led_on(PROTOBOARD_BLUE_LED_PORT,PROTOBOARD_BLUE_LED_PIN)
#define PROTOBOARD_BLUE_LED_OFF		led_off(PROTOBOARD_BLUE_LED_PORT,PROTOBOARD_BLUE_LED_PIN)

#define PROTOBOARD_WHITE_LED_PORT	GPIOA
#define PROTOBOARD_WHITE_LED_PIN	GPIO_PIN_NO_2
#define PROTOBOARD_WHITE_LED_ON	led_on(PROTOBOARD_WHITE_LED_PORT,PROTOBOARD_WHITE_LED_PIN)
#define PROTOBOARD_WHITE_LED_OFF	led_off(PROTOBOARD_WHITE_LED_PORT,PROTOBOARD_WHITE_LED_PIN)

#define JOYSTICK_ADC				ADC1
#define JOYSTICK_X_ADC_CHANNEL		ADC_CHANNEL_5
#define JOYSTICK_Y_ADC_CHANNEL		ADC_CHANNEL_7

#define RTE_PLAYER_INITIAL_SPEED		0
#define RTE_PLAYER_BASE_ACCELERATION	1
#define RTE_PLAYER_BASE_DECELERATION	0.1
#define RTE_PLAYER_MAX_SPEED			5
#define RTE_ASTEROID_BASE_SPEED			1
#define RTE_ROCKET_BASE_SPEED			3

#define RTE_ROCKET_LIFESPAN	30

#define RTE_ASTEROID_SIZE_L	0
#define RTE_ASTEROID_SIZE_M	1

#define RTE_HEADING_DIR_N			1
#define RTE_HEADING_DIR_S			2
#define RTE_HEADING_DIR_E			3
#define RTE_HEADING_DIR_W			4	
#define RTE_HEADING_DIR_NE			5	
#define RTE_HEADING_DIR_NW			6
#define RTE_HEADING_DIR_SE			7	
#define RTE_HEADING_DIR_SW			8

#define RTE_ASTEROID_BMP_W	 		50
#define RTE_ASTEROID_BMP_H			42
#define RTE_ASTEROID_MEDIUM_BMP_W	27
#define RTE_ASTEROID_MEDIUM_BMP_H	25

#define RTE_PLAYER_SPACESHIP_BMP_W1			39
#define RTE_PLAYER_SPACESHIP_BMP_H1			39

#define RTE_PLAYER_SPACESHIP_BMP_W2			41
#define RTE_PLAYER_SPACESHIP_BMP_H2			41

#define RTE_ROCKET_BMP_W1		13
#define RTE_ROCKET_BMP_H1		22

#define RTE_ROCKET_BMP_W2		22
#define RTE_ROCKET_BMP_H2		22

#define RTE_ROCKET_N_START_X	(RTE_PLAYER_SPACESHIP_BMP_W1)/2 - 5
#define RTE_ROCKET_N_START_Y	-RTE_ROCKET_BMP_H1
#define RTE_ROCKET_S_START_X	(RTE_PLAYER_SPACESHIP_BMP_W1)/2 	- 5
#define RTE_ROCKET_S_START_Y	RTE_PLAYER_SPACESHIP_BMP_H1
#define RTE_ROCKET_E_START_X	RTE_PLAYER_SPACESHIP_BMP_W1
#define RTE_ROCKET_E_START_Y	(RTE_PLAYER_SPACESHIP_BMP_H1)/2 - 5
#define RTE_ROCKET_W_START_X	-RTE_ROCKET_BMP_H1
#define RTE_ROCKET_W_START_Y	(RTE_PLAYER_SPACESHIP_BMP_H1)/2 - 5

#define RTE_ROCKET_NE_START_X	RTE_PLAYER_SPACESHIP_BMP_W2
#define RTE_ROCKET_NE_START_Y	-RTE_ROCKET_BMP_H2
#define RTE_ROCKET_SE_START_X	RTE_PLAYER_SPACESHIP_BMP_W2
#define RTE_ROCKET_SE_START_Y	RTE_PLAYER_SPACESHIP_BMP_H2
#define RTE_ROCKET_NW_START_X	-RTE_ROCKET_BMP_W2
#define RTE_ROCKET_NW_START_Y	-RTE_ROCKET_BMP_H2
#define RTE_ROCKET_SW_START_X	-RTE_ROCKET_BMP_W2
#define RTE_ROCKET_SW_START_Y	RTE_PLAYER_SPACESHIP_BMP_H2

#define RTE_ALIVE_FALSE		2
#define RTE_ALIVE_TRUE 		1
#define RTE_ALIVE_UNSET 	0

#define RTE_DEAD_OBJECT_UNCLEARED	0
#define RTE_DEAD_OBJECT_CLEARED		1

#define RTE_COLLISION_TRUE 		1
#define RTE_COLLISION_FALSE 	0

#define RTE_FIRST_TIME_TRUE 	1
#define RTE_FIRST_TIME_FALSE	0

#define RTE_ASTEROID_BUFFER_SIZE 	15
#define RTE_ROCKET_BUFFER_SIZE		3

#define RTE_MARGIN 0

#define RTE_NUM_OF_WAVE	5

/***********************************************************************
Structure definition
***********************************************************************/
typedef struct{
	int16_t x;
	int16_t y;
	double dx;
	double dy;
	uint8_t headingDir;
	uint8_t aliveFlag;
	uint8_t lifeSpan;
	uint8_t asteroidSize;
}Object_Property_t;

typedef struct{
	uint8_t imageWidth;
	uint8_t imageHeight;
	const uint8_t *image;
	uint8_t clearWhenDead;
}Object_Image_t;

typedef struct{
	Object_Property_t Object_Property;
	Object_Image_t Object_Image;
}Space_Object_t;

/***********************************************************************
Function prototype
***********************************************************************/
void RTE_init (void);

void RTE_start_update_frame (void);

void RTE_create_player_spaceship (Space_Object_t *PlayerSpaceShipPtr);
void RTE_create_asteroid (vector *AsteroidVectPtr,Space_Object_t *AsteroidPtr, uint8_t numberToCreate, Space_Object_t *PlayerSpaceShipPtr);
void RTE_create_rocket (vector *RocketVectPtr, Space_Object_t *RocketPtr, Space_Object_t *PlayerSpaceShipPtr);

void RTE_draw_player_spaceship (Space_Object_t *PlayerSpaceShipPtr);
void RTE_draw_asteroid (vector *AsteroidVectPtr);
void RTE_draw_rocket (vector *RocketVectPtr);

void RTE_update_player_spaceship (Space_Object_t *PlayerSpaceShipPtr);
void RTE_update_asteroid (vector *AsteroidVectPtr, Space_Object_t *PlayerSpaceShipPtr);
void RTE_update_rocket (vector *RocketVectPtr, vector *AsteroidVectPtr);

void RTE_display_black_background(void);
void RTE_display_start_screen(void);
void RTE_display_score(void);
void RTE_display_game_over_screen(void);

void RTE_reset_game(void);

#endif
