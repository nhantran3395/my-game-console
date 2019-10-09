/**
*@file 	game_engine.c
*@brief "Return To Earth" game engine library.
*
*This header file provide functions for making handheld console game "Return To Earth".
*
*@author Tran Thanh Nhan
*@date 09/09/2019
*/

#include "game_engine.h"
#include "../Miscellaneous/inc/bitmap_byte_array.h"
#include "../Miscellaneous/inc/rocket_launch.h"
#include "../Miscellaneous/inc/spaceship_explode.h"
#include "../Miscellaneous/inc/spaceship_thruster.h"
#include "../Miscellaneous/inc/asteroid_impact.h"
#include "../Miscellaneous/inc/asteroid_large_explode.h"
#include "../Miscellaneous/inc/asteroid_medium_explode.h"

/***********************************************************************
External function prototype
***********************************************************************/
extern ILI9341_Config_t ILI9341_config;
extern void ILI9341_send_command (uint8_t cmd);
extern void ILI9341_send_parameter_16_bits (uint16_t param);
extern void ILI9341_set_active_area (uint16_t startColum, uint16_t startPage, uint16_t endColumn, uint16_t endPage);

/***********************************************************************
Private function prototype
***********************************************************************/
int16_t RTE_random_x (void);
int16_t RTE_random_y (void);
int8_t RTE_random_sign (void); 
void RTE_wrap_cordinate (int16_t *xPtr, int16_t *yPtr);
void RTE_update_player_spaceship_direction (Space_Object_t *PlayerSpaceShipPtr);
void RTE_update_player_spaceship_position (Space_Object_t *PlayerSpaceShipPtr);
void RTE_delete_dead_rocket (Space_Object_t *RocketPtr);
void RTE_delete_dead_asteroid (Space_Object_t *AsteroidPtr);
uint8_t RTE_collision_detect (Space_Object_t *Object1Ptr, Space_Object_t *Object2Ptr);
void RTE_accelerate_player_spaceship (Space_Object_t *PlayerSpaceShipPtr, int8_t ddx, int8_t ddy);
void RTE_create_medium_asteroid (vector *AsteroidVectPtr, Space_Object_t *DeadAsteroidPtr);

/***********************************************************************
Global variable
***********************************************************************/
uint8_t frameUpdate = CLEAR;
int16_t score = 0;
int16_t scorePrevious = 0;
char displayScore[15];

uint8_t shootButtonFirstTimeFlag = RTE_FIRST_TIME_TRUE;

Space_Object_t PlayerSpaceship;
volatile Space_Object_t Asteroid[RTE_ASTEROID_BUFFER_SIZE] ;
Space_Object_t Rocket[RTE_ROCKET_BUFFER_SIZE];

vector AsteroidVect;
vector RocketVect;

uint8_t currentWave = 0;
uint8_t numOfAsteroidInWave[RTE_NUM_OF_WAVE] = {1,2,3,4,5};

/***********************************************************************
Public function: Initialize game engine
***********************************************************************/
void RTE_init (void)
{
	RCC_set_SYSCLK_PLL_84_MHz();
	
	RNG_init();
	
	ILI9341_init();
	ILI9341_rotate(ILI9341_orientation_landscape_2);
	ILI9341_fill_display(ILI9341_BLACK);
	
	joystick_init(JOYSTICK_ADC,JOYSTICK_X_ADC_CHANNEL,JOYSTICK_Y_ADC_CHANNEL);
	
	speaker_init(DAC_CHANNEL_1,9,679);

	button_init(SHOOT_BUTTON_PORT,SHOOT_BUTTON_PIN,GPIO_PU);
	button_init(THRUST_BUTTON_PORT,THRUST_BUTTON_PIN,GPIO_PU);
	
	led_init(PROTOBOARD_RED_LED_PORT,PROTOBOARD_RED_LED_PIN);
	led_init(PROTOBOARD_GREEN_LED_PORT,PROTOBOARD_GREEN_LED_PIN);
	led_init(PROTOBOARD_BLUE_LED_PORT,PROTOBOARD_BLUE_LED_PIN);
	led_init(PROTOBOARD_WHITE_LED_PORT,PROTOBOARD_WHITE_LED_PIN);
	
	/*configure timer 6 to generate periodic interrupt of 33ms (screen refresh rate 30Hz)*/
	TIM_init_direct(TIM6,1874,999);
	TIM_intrpt_vector_ctr(IRQ_TIM6_DAC,ENABLE);
	TIM_interrupt_ctr(TIM6,ENABLE);
	
	/*configure timer 3 (timer  3 is used for button debouncing)*/
	TIM_init_direct(TIM3,59999,999);
	TIM_intrpt_vector_ctr(IRQ_TIM3,ENABLE);
	TIM_interrupt_ctr(TIM3,ENABLE);

	/*initialize asteroid vector*/
	vector_init(&AsteroidVect);

	/*initialize rocket vector*/
	vector_init(&RocketVect);

}

/***********************************************************************
Public function: Start updating game screen
***********************************************************************/
void RTE_start_update_frame (void)
{
	TIM_ctr(TIM6,START);
}

/***********************************************************************
Public function: Create player spaceship (Fill data into player spaceship structure)
***********************************************************************/
void RTE_create_player_spaceship (Space_Object_t *PlayerSpaceShipPtr)
{
	PlayerSpaceShipPtr->Object_Property.x = RTE_random_x();
	PlayerSpaceShipPtr->Object_Property.y = RTE_random_y();	
	
	PlayerSpaceShipPtr->Object_Property.dx = 0;
	PlayerSpaceShipPtr->Object_Property.dy = 0;	
	
	PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_N;
	
	PlayerSpaceShipPtr->Object_Property.aliveFlag = RTE_ALIVE_TRUE;
	PlayerSpaceShipPtr->Object_Property.lifeSpan = 0;
	
	PlayerSpaceShipPtr->Object_Image.image = player_spaceship_north_bmp;
	PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H1;
	PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W1;
}

/***********************************************************************
Public function: Create asteroid (Fill data into elements in array of asteroid structure, then add to asteroid vector)
***********************************************************************/
void RTE_create_asteroid (vector *AsteroidVectPtr,Space_Object_t *AsteroidPtr, uint8_t numberToCreate, Space_Object_t *PlayerSpaceShipPtr)
{
	Space_Object_t *PreviousAsteroidPtr = NULL;
	uint8_t check1 = 0, check2 =0;

	for (uint8_t count = 0; count < numberToCreate; count++,AsteroidPtr++){

		/*store address of asteroid to be created in asteroid vector*/
		vector_add(AsteroidVectPtr,(void*)AsteroidPtr);

		AsteroidPtr->Object_Image.image = asteroid_bmp;
		AsteroidPtr->Object_Image.imageHeight = RTE_ASTEROID_BMP_H;
		AsteroidPtr->Object_Image.imageWidth = RTE_ASTEROID_BMP_W;
		AsteroidPtr->Object_Image.clearWhenDead = RTE_DEAD_OBJECT_UNCLEARED;

		/*keep randomizing asteroid 's position until asteroid being generated not colliding with player and also with other asteroids*/
		do{
			check1 = RTE_COLLISION_FALSE;
			check2 = RTE_COLLISION_FALSE;

			AsteroidPtr->Object_Property.x = RTE_random_x();
			AsteroidPtr->Object_Property.y = RTE_random_y();

			check1 = RTE_collision_detect(AsteroidPtr,PlayerSpaceShipPtr);

			if(check1 == RTE_COLLISION_TRUE){
				continue;
			}


			/*if first asteroid is being created, no need to check for collision with other asteroid*/
			if(count == 0){
				break;
			}

			/*for generation of 2nd,3rd and other asteroid, collision need to be check*/
			PreviousAsteroidPtr = AsteroidPtr-1;

			for(uint8_t i = 0; i< count;i++,PreviousAsteroidPtr--){
				check2 = RTE_collision_detect(AsteroidPtr,PreviousAsteroidPtr);

				if(check2 == RTE_COLLISION_TRUE){
					break;
				}
			}

		}while((check1 == RTE_COLLISION_TRUE) || (check2 == RTE_COLLISION_TRUE ));

		AsteroidPtr->Object_Property.dx = RTE_random_sign()*RTE_ASTEROID_BASE_SPEED;
		AsteroidPtr->Object_Property.dy = RTE_random_sign()*RTE_ASTEROID_BASE_SPEED;

		AsteroidPtr->Object_Property.aliveFlag = RTE_ALIVE_TRUE;
		AsteroidPtr->Object_Property.lifeSpan = 0;
		AsteroidPtr->Object_Property.asteroidSize = RTE_ASTEROID_SIZE_L;
	}
}

/***********************************************************************
Public function: Create rocket (Fill data into elements in array of rocket structure, then add to rocket vector)
***********************************************************************/
void RTE_create_rocket (vector *RocketVectPtr, Space_Object_t *RocketPtr, Space_Object_t *PlayerSpaceShipPtr)
{
	if (!SHOOT_BUTTON_READ){

		if(shootButtonFirstTimeFlag == RTE_FIRST_TIME_TRUE){

			TIM_ctr(TIM3,START);

			shootButtonFirstTimeFlag = RTE_FIRST_TIME_FALSE;

			PROTOBOARD_WHITE_LED_ON;

			speaker_play_sound(rocket_launch,sizeof(rocket_launch)/sizeof(rocket_launch[0]));

			scorePrevious = score;
			score--;

			Space_Object_t *head = RocketPtr;

			while((RocketPtr->Object_Property.aliveFlag != RTE_ALIVE_FALSE)	&& (RocketPtr->Object_Property.aliveFlag != RTE_ALIVE_UNSET)){
				RocketPtr ++;
				if(RocketPtr - head > (RTE_ROCKET_BUFFER_SIZE-1)){
					return;
				}
			}

			vector_add(RocketVectPtr,RocketPtr);

			RocketPtr->Object_Property.aliveFlag = RTE_ALIVE_TRUE;
			RocketPtr->Object_Property.lifeSpan = RTE_ROCKET_LIFESPAN;
			RocketPtr->Object_Image.clearWhenDead = RTE_DEAD_OBJECT_UNCLEARED;

			if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_N){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_N_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_N_START_Y;

				RocketPtr->Object_Property.dx = 0;
				RocketPtr->Object_Property.dy = -RTE_ROCKET_BASE_SPEED;

				RocketPtr->Object_Image.image = rocket_north_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_W1;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_H1;

			}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_S){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_S_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_S_START_Y;

				RocketPtr->Object_Property.dx =	0;
				RocketPtr->Object_Property.dy = RTE_ROCKET_BASE_SPEED;

				RocketPtr->Object_Image.image = rocket_south_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_W1;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_H1;

			}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_E){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_E_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_E_START_Y;

				RocketPtr->Object_Property.dx = RTE_ROCKET_BASE_SPEED;
				RocketPtr->Object_Property.dy = 0;

				RocketPtr->Object_Image.image = rocket_east_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_H1;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_W1;

			}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_W){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_W_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_W_START_Y;

				RocketPtr->Object_Property.dx = -RTE_ROCKET_BASE_SPEED;
				RocketPtr->Object_Property.dy = 0;

				RocketPtr->Object_Image.image = rocket_west_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_H1;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_W1;

			}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_NE){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_NE_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_NE_START_Y;

				RocketPtr->Object_Property.dx = RTE_ROCKET_BASE_SPEED;
				RocketPtr->Object_Property.dy = -RTE_ROCKET_BASE_SPEED;

				RocketPtr->Object_Image.image = rocket_north_east_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_W2;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_H2;

			}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_SE){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_SE_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_SE_START_Y;

				RocketPtr->Object_Property.dx = RTE_ROCKET_BASE_SPEED;
				RocketPtr->Object_Property.dy = RTE_ROCKET_BASE_SPEED;

				RocketPtr->Object_Image.image = rocket_south_east_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_W2;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_H2;

			}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_NW){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_NW_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_NW_START_Y;

				RocketPtr->Object_Property.dx = -RTE_ROCKET_BASE_SPEED;
				RocketPtr->Object_Property.dy = -RTE_ROCKET_BASE_SPEED;

				RocketPtr->Object_Image.image = rocket_north_west_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_W2;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_H2;

			}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_SW){

				RocketPtr->Object_Property.x = PlayerSpaceShipPtr->Object_Property.x + RTE_ROCKET_SW_START_X;
				RocketPtr->Object_Property.y = PlayerSpaceShipPtr->Object_Property.y + RTE_ROCKET_SW_START_Y;

				RocketPtr->Object_Property.dx = -RTE_ROCKET_BASE_SPEED;
				RocketPtr->Object_Property.dy = RTE_ROCKET_BASE_SPEED;

				RocketPtr->Object_Image.image = rocket_south_west_bmp;
				RocketPtr->Object_Image.imageWidth = RTE_ROCKET_BMP_W2;
				RocketPtr->Object_Image.imageHeight = RTE_ROCKET_BMP_H2;

			}
		}
	}else{
		PROTOBOARD_WHITE_LED_OFF;
	}
}

/***********************************************************************
Public function: Draw player spaceship
***********************************************************************/
void RTE_draw_player_spaceship (Space_Object_t *PlayerSpaceShipPtr)
{
	ILI9341_draw_bitmap_w_background(PlayerSpaceShipPtr->Object_Property.x,PlayerSpaceShipPtr->Object_Property.y,
	PlayerSpaceShipPtr->Object_Image.image,PlayerSpaceShipPtr->Object_Image.imageWidth,
	PlayerSpaceShipPtr->Object_Image.imageHeight,ILI9341_LIGHTGREY,ILI9341_BLACK);
}

/***********************************************************************
Public function: Draw active asteroid (asteroid which address is store in asteroid vector)
***********************************************************************/
void RTE_draw_asteroid (vector *AsteroidVect)
{
	Space_Object_t *AsteroidPtr = NULL;
	for(uint8_t count = 0;count < AsteroidVect->total;count++){
		AsteroidPtr = vector_get(AsteroidVect,count);
		ILI9341_draw_bitmap_w_background(AsteroidPtr->Object_Property.x,AsteroidPtr->Object_Property.y,
		AsteroidPtr->Object_Image.image,AsteroidPtr->Object_Image.imageWidth,
		AsteroidPtr->Object_Image.imageHeight,0xB3E7,ILI9341_BLACK);
	}
}

/***********************************************************************
Public function: Draw active rocket (rocket which address is store in rocket vector)
***********************************************************************/
void RTE_draw_rocket (vector *RocketVectPtr)
{
	Space_Object_t  *RocketPtr = NULL;
	for (uint8_t count = 0;count < RocketVectPtr->total;count++){
		RocketPtr = vector_get(RocketVectPtr,count);
		ILI9341_draw_bitmap_w_background(RocketPtr->Object_Property.x,RocketPtr->Object_Property.y,
		RocketPtr->Object_Image.image,RocketPtr->Object_Image.imageWidth,
		RocketPtr->Object_Image.imageHeight,ILI9341_LIGHTGREY,ILI9341_BLACK);
	}
}


/***********************************************************************
Public function: Update player spaceship 's information
***********************************************************************/
void RTE_update_player_spaceship (Space_Object_t *PlayerSpaceShipPtr)
{
	RTE_update_player_spaceship_direction (PlayerSpaceShipPtr);
	RTE_update_player_spaceship_position	(PlayerSpaceShipPtr);
}

/***********************************************************************
Public function: Update information of active asteroid
***********************************************************************/
void RTE_update_asteroid (vector *AsteroidVectPtr, Space_Object_t *PlayerSpaceShipPtr)
{
	Space_Object_t *AsteroidPtr = NULL;
	Space_Object_t *OtherAsteroidPtr = NULL;

	for(uint8_t count = 0;count < AsteroidVectPtr->total;count++){

		AsteroidPtr = vector_get(AsteroidVectPtr,count);

		/*update current asteroid 's position*/
		AsteroidPtr->Object_Property.x += AsteroidPtr->Object_Property.dx;
		AsteroidPtr->Object_Property.y += AsteroidPtr->Object_Property.dy;
		RTE_wrap_cordinate(&AsteroidPtr->Object_Property.x,&AsteroidPtr->Object_Property.y);

		/*check whether current asteroid collide with other asteroid (if there are more than 1 active asteroid)*/
		if(AsteroidVectPtr->total > 1){

			for (uint8_t i = count + 1;i < AsteroidVectPtr->total;i++){

				OtherAsteroidPtr = vector_get(AsteroidVectPtr,i);

				if(OtherAsteroidPtr->Object_Property.aliveFlag == RTE_ALIVE_TRUE){
					if(RTE_collision_detect(AsteroidPtr,OtherAsteroidPtr) == RTE_COLLISION_TRUE){

						/*if collided make both asteroid travel in opposite direction*/
						AsteroidPtr->Object_Property.dx *= -1;
						AsteroidPtr->Object_Property.dy *= -1;
						OtherAsteroidPtr->Object_Property.dx *= -1;
						OtherAsteroidPtr->Object_Property.dy *= -1;

						speaker_play_sound(asteroid_impact,sizeof(asteroid_impact)/sizeof(asteroid_impact[0]));
					}
				}
			}
		}

		/*check whether current asteroid and player spaceship collided*/
		if(RTE_collision_detect(AsteroidPtr,PlayerSpaceShipPtr) == RTE_COLLISION_TRUE){

			/*if collided mark player spaceship as dead and return to main loop*/
			PlayerSpaceShipPtr->Object_Property.aliveFlag = RTE_ALIVE_FALSE;

			speaker_play_sound(spaceship_explode,sizeof(spaceship_explode)/sizeof(spaceship_explode[0]));

			return;
		}
	}
}

/***********************************************************************
Function: Update active rocket information
***********************************************************************/
void RTE_update_rocket (vector *RocketVectPtr, vector *AsteroidVectPtr)
{
	Space_Object_t *RocketPtr = NULL;
	Space_Object_t *AsteroidPtr = NULL;

	for(int8_t count = 0; count < RocketVectPtr->total; count++){

		RocketPtr = vector_get(RocketVectPtr,count);

		RocketPtr->Object_Property.lifeSpan--;

		RocketPtr->Object_Property.x += RocketPtr->Object_Property.dx;
		RocketPtr->Object_Property.y += RocketPtr->Object_Property.dy;
		RTE_wrap_cordinate(&RocketPtr->Object_Property.x,&RocketPtr->Object_Property.y);

		/*if life span of rocket ran out, mark rocket as dead and remove from rocket vector */
		if(!(RocketPtr->Object_Property.lifeSpan)){
			RocketPtr->Object_Property.aliveFlag = RTE_ALIVE_FALSE;
			RTE_delete_dead_rocket(RocketPtr);
			vector_delete(RocketVectPtr,count);
			count--;
		}

		/*if rocket hit an asteroid, mark both rocket and asteroid as dead, remove current rocket from rocket vector, asteroid from asteroid vector*/
		for(int8_t i = 0;i < AsteroidVectPtr->total; i++){
			AsteroidPtr = (Space_Object_t*)vector_get(AsteroidVectPtr,i);

			if(RTE_collision_detect(RocketPtr,AsteroidPtr) == RTE_COLLISION_TRUE){

				scorePrevious = score;
				score += 10;

				RocketPtr->Object_Property.aliveFlag = RTE_ALIVE_FALSE;
				RTE_delete_dead_rocket(RocketPtr);
				vector_delete(RocketVectPtr,count);
				count--;
				AsteroidPtr->Object_Property.aliveFlag = RTE_ALIVE_FALSE;
				RTE_delete_dead_asteroid(AsteroidPtr);
				vector_delete(AsteroidVectPtr,i);
				i--;

				/*if asteroid that was hit is large one, create 2 medium asteroids*/
				if(AsteroidPtr->Object_Property.asteroidSize == RTE_ASTEROID_SIZE_L){
					RTE_create_medium_asteroid(AsteroidVectPtr,AsteroidPtr);
					speaker_play_sound(asteroid_large_explode,sizeof(asteroid_large_explode)/sizeof(asteroid_large_explode[0]));
					break;
				}else if (AsteroidPtr->Object_Property.asteroidSize == RTE_ASTEROID_SIZE_M){
					speaker_play_sound(asteroid_medium_explode,sizeof(asteroid_medium_explode)/sizeof(asteroid_medium_explode[0]));
				}
			}

		}
	}
}

/***********************************************************************
Function: Display player score
***********************************************************************/
void RTE_display_score (void)
{
	char displayScore[15];
	sprintf(displayScore,"Score: %d",scorePrevious);

	/*if score is updated, clear old score before printing new score to screen*/
	if(scorePrevious |= score){
		ILI9341_put_string(240,0,displayScore,&TM_Font_7x10,ILI9341_BLACK);
		sprintf(displayScore,"Score: %d",score);
		ILI9341_put_string(240,0,displayScore,&TM_Font_7x10,ILI9341_YELLOW);
	}else{
		ILI9341_put_string(240,0,displayScore,&TM_Font_7x10,ILI9341_YELLOW);
	}
}

/***********************************************************************
Function: Display start screen
***********************************************************************/
void RTE_display_start_screen(void)
{
	RTE_display_black_background();
	ILI9341_draw_bitmap(48,8,earth_bmp,225,225,ILI9341_DARKCYAN);
	ILI9341_put_string(40,60,"RETURN TO EARTH",&TM_Font_16x26,ILI9341_WHITE);
	ILI9341_put_string(20,220,"Press shoot button to start",&TM_Font_11x18,ILI9341_WHITE);	
}

/***********************************************************************
Function: Display game over screen
***********************************************************************/
void RTE_display_game_over_screen(void)
{
	RTE_display_black_background();
	ILI9341_draw_bitmap(48,0,meteor_bmp,225,225,ILI9341_YELLOW);
	ILI9341_put_string(10,200,"Uh oh,your space ship burned down.Want to try again?",&TM_Font_11x18,ILI9341_WHITE);
}

/***********************************************************************
Function: Preparation before resetting game
***********************************************************************/
void RTE_reset_game(void)
{
	score = 0;
	frameUpdate = CLEAR;
	RNG_init();
	TIM_ctr(TIM6,STOP);
	currentWave = 0;

	for(uint8_t count = 0; count < AsteroidVect.total;){
		vector_delete(&AsteroidVect,count);
	}

	for(uint8_t count = 0; count < RocketVect.total;){
		vector_delete(&RocketVect,count);
	}

	for(uint8_t count = 0;count < RTE_ASTEROID_BUFFER_SIZE;count++){
		Asteroid[count].Object_Property.aliveFlag = RTE_ALIVE_FALSE;
	}

	for(uint8_t count = 0;count < RTE_ROCKET_BUFFER_SIZE;count++){
		Rocket[count].Object_Property.aliveFlag = RTE_ALIVE_FALSE;
	}
}

/***********************************************************************
Private function: Wrap coordinate
***********************************************************************/
void RTE_wrap_cordinate (int16_t *xPtr, int16_t *yPtr)
{
	if (*xPtr < 0){
		*xPtr += ILI9341_config.width;
	}
	
	if (*xPtr >=  ILI9341_config.width){
		*xPtr -= ILI9341_config.width;
	}

	if (*yPtr < 0){
		*yPtr += ILI9341_config.height;
	}
	
	if (*yPtr >=  ILI9341_config.height){
		*yPtr -= ILI9341_config.height;
	}
}

/***********************************************************************
Private function: Random object initial x value
***********************************************************************/
int16_t RTE_random_x (void)
{
	return (RNG_get() & 0x1FF) % (ILI9341_config.width +1);
}

/***********************************************************************
Private function: Random object initial y value
***********************************************************************/
int16_t RTE_random_y (void)
{
	return (RNG_get() & 0xFF) % (ILI9341_config.height +1);
}


/***********************************************************************
Private function: Randomize sign (minus or plus)
***********************************************************************/
int8_t RTE_random_sign (void)
{
	uint8_t temp = (RNG_get() & 0x0F) % 9;
	if(temp < 5){
		return -1;
	}
	return 1;
}

/***********************************************************************
Private function: Update player spaceship direction
***********************************************************************/
void RTE_update_player_spaceship_direction (Space_Object_t *PlayerSpaceShipPtr)
{
	uint8_t direction = 0;
	direction = joystick_read_direction (JOYSTICK_ADC,JOYSTICK_X_ADC_CHANNEL, JOYSTICK_Y_ADC_CHANNEL);
	
	if (direction == JS_DIR_CENTERED){
		return;
	}else if (direction == JS_DIR_UP){
		
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_N;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_north_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W1;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H1;
		return;
	}else if (direction == JS_DIR_DOWN){
				
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_S;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_south_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W1;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H1;
		return;
		
	}else if (direction == JS_DIR_RIGHT){
		
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_E;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_east_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W1;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H1;
		return;
		
	}else if (direction == JS_DIR_LEFT){
		
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_W;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_west_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W1;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H1;
		return;
		
	}else if (direction == JS_DIR_RIGHT_UP){
		
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_NE;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_north_east_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W2;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H2;
		return;
		
	}else if (direction == JS_DIR_RIGHT_DOWN){
		
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_SE;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_south_east_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W2;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H2;
		return;
		
	}else if (direction == JS_DIR_LEFT_UP){
		
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_NW;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_north_west_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W2;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H2;
		return;
		
	}else if (direction == JS_DIR_LEFT_DOWN){
		
		PlayerSpaceShipPtr->Object_Property.headingDir = RTE_HEADING_DIR_SW;
		
		PlayerSpaceShipPtr->Object_Image.image = player_spaceship_south_west_bmp;
		PlayerSpaceShipPtr->Object_Image.imageWidth = RTE_PLAYER_SPACESHIP_BMP_W2;
		PlayerSpaceShipPtr->Object_Image.imageHeight = RTE_PLAYER_SPACESHIP_BMP_H2;
		return;
		
	}
}

/***********************************************************************
Private function: Update player spaceship position
***********************************************************************/
void RTE_update_player_spaceship_position (Space_Object_t *PlayerSpaceShipPtr)
{
	
	if(!THRUST_BUTTON_READ){
		
		int8_t ddx = 0;
		int8_t ddy = 0;

		PROTOBOARD_BLUE_LED_ON;
		
		speaker_play_sound(spaceship_thruster,sizeof(spaceship_thruster)/sizeof(spaceship_thruster[0]));

		if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_N){
			ddx	= 0;
			ddy = -RTE_PLAYER_BASE_ACCELERATION;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_S){
			ddx += 0;
			ddy += RTE_PLAYER_BASE_ACCELERATION;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_E){
			ddx += RTE_PLAYER_BASE_ACCELERATION;
			ddy += 0;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_W){
			ddx += -RTE_PLAYER_BASE_ACCELERATION;
			ddy += 0;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_NE){
			ddx += RTE_PLAYER_BASE_ACCELERATION;
			ddy += -RTE_PLAYER_BASE_ACCELERATION;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_NW){
			ddx += -RTE_PLAYER_BASE_ACCELERATION;
			ddy += -RTE_PLAYER_BASE_ACCELERATION;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_SE){
			ddx += RTE_PLAYER_BASE_ACCELERATION;
			ddy += RTE_PLAYER_BASE_ACCELERATION;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}else if(PlayerSpaceShipPtr->Object_Property.headingDir == RTE_HEADING_DIR_SW){
			ddx += -RTE_PLAYER_BASE_ACCELERATION;
			ddy += RTE_PLAYER_BASE_ACCELERATION;
			RTE_accelerate_player_spaceship(PlayerSpaceShipPtr,ddx,ddy);
		}
	
	}else{
		
		PROTOBOARD_BLUE_LED_OFF;

		PlayerSpaceShipPtr->Object_Property.x += PlayerSpaceShipPtr->Object_Property.dx;
		PlayerSpaceShipPtr->Object_Property.y += PlayerSpaceShipPtr->Object_Property.dy;
		RTE_wrap_cordinate(&PlayerSpaceShipPtr->Object_Property.x,&PlayerSpaceShipPtr->Object_Property.y);
		
		/*decelerate player spaceship*/
		
		if(fabs(PlayerSpaceShipPtr->Object_Property.dx) > RTE_PLAYER_BASE_DECELERATION){ 
			
			if(PlayerSpaceShipPtr->Object_Property.dx > 0){
				PlayerSpaceShipPtr->Object_Property.dx -= RTE_PLAYER_BASE_DECELERATION;
			}else if(PlayerSpaceShipPtr->Object_Property.dx < 0){
				PlayerSpaceShipPtr->Object_Property.dx += RTE_PLAYER_BASE_DECELERATION;
			}
		}else{
			PlayerSpaceShipPtr->Object_Property.dx = 0;
		}
		
		if(fabs(PlayerSpaceShipPtr->Object_Property.dy) > RTE_PLAYER_BASE_DECELERATION){		
						
			if(PlayerSpaceShipPtr->Object_Property.dy > 0){
				PlayerSpaceShipPtr->Object_Property.dy -= RTE_PLAYER_BASE_DECELERATION;
			}else if(PlayerSpaceShipPtr->Object_Property.dy < 0){
				PlayerSpaceShipPtr->Object_Property.dy += RTE_PLAYER_BASE_DECELERATION;
			}
		}else{
			PlayerSpaceShipPtr->Object_Property.dy = 0;
		}
	
	}
	
}

/***********************************************************************
Private function: Detect collision between 2 object using AABB algorithm
***********************************************************************/
uint8_t RTE_collision_detect (Space_Object_t *Object1Ptr, Space_Object_t *Object2Ptr)
{
	int16_t Obj1BottomRight_X = Object1Ptr->Object_Property.x + Object1Ptr->Object_Image.imageWidth;
	int16_t Obj1BottomRight_Y = Object1Ptr->Object_Property.y + Object1Ptr->Object_Image.imageHeight;
	
	int16_t Obj2BottomRight_X = Object2Ptr->Object_Property.x + Object2Ptr->Object_Image.imageWidth;
	int16_t Obj2BottomRight_Y = Object2Ptr->Object_Property.y + Object2Ptr->Object_Image.imageHeight;
	
	if (Object1Ptr->Object_Property.x < Obj2BottomRight_X 
		&& Object2Ptr->Object_Property.x < Obj1BottomRight_X
		&& Object1Ptr->Object_Property.y < Obj2BottomRight_Y
		&& Object2Ptr->Object_Property.y < Obj1BottomRight_Y){
						
		return RTE_COLLISION_TRUE;
	}	

	return RTE_COLLISION_FALSE;
}

/***********************************************************************
Private function: Delete dead rocket
***********************************************************************/
void RTE_delete_dead_rocket (Space_Object_t *RocketPtr)
{
		if((RocketPtr->Object_Property.aliveFlag == RTE_ALIVE_FALSE) && (RocketPtr->Object_Image.clearWhenDead == RTE_DEAD_OBJECT_UNCLEARED)){
			ILI9341_draw_bitmap_w_background(RocketPtr->Object_Property.x,RocketPtr->Object_Property.y,
			RocketPtr->Object_Image.image,RocketPtr->Object_Image.imageWidth,
			RocketPtr->Object_Image.imageHeight,ILI9341_BLACK,ILI9341_BLACK);
			RocketPtr->Object_Image.clearWhenDead = RTE_DEAD_OBJECT_CLEARED;
		}
}


/***********************************************************************
Private function: Delete dead rocket
***********************************************************************/
void RTE_delete_dead_asteroid (Space_Object_t *AsteroidPtr)
{
		if((AsteroidPtr->Object_Property.aliveFlag == RTE_ALIVE_FALSE) && (AsteroidPtr->Object_Image.clearWhenDead == RTE_DEAD_OBJECT_UNCLEARED)){
			ILI9341_draw_bitmap_w_background(AsteroidPtr->Object_Property.x,AsteroidPtr->Object_Property.y,
			AsteroidPtr->Object_Image.image,AsteroidPtr->Object_Image.imageWidth,
			AsteroidPtr->Object_Image.imageHeight,ILI9341_BLACK,ILI9341_BLACK);
			AsteroidPtr->Object_Image.clearWhenDead = RTE_DEAD_OBJECT_CLEARED;
		}
}

/***********************************************************************
Private function: Accelerate player spaceship
***********************************************************************/
void RTE_accelerate_player_spaceship (Space_Object_t *PlayerSpaceShipPtr, int8_t ddx, int8_t ddy)
{
	if(fabs(PlayerSpaceShipPtr->Object_Property.dx) < RTE_PLAYER_MAX_SPEED){
		PlayerSpaceShipPtr->Object_Property.dx += ddx;
	}

	if(fabs(PlayerSpaceShipPtr->Object_Property.dy) < RTE_PLAYER_MAX_SPEED){
		PlayerSpaceShipPtr->Object_Property.dy += ddy;
	}
}

/***********************************************************************
Private function: Create 2 medium size asteroids
***********************************************************************/
void RTE_create_medium_asteroid (vector *AsteroidVectPtr, Space_Object_t *DeadAsteroidPtr)
{
	Space_Object_t *AsteroidPtr = Asteroid;

	uint8_t deadAsteroid_x =  DeadAsteroidPtr->Object_Property.x;
	uint8_t deadAsteroid_y =  DeadAsteroidPtr->Object_Property.y;

	RNG_init();

	for(uint8_t j =0; j < 2; j++){

		/*find element in asteroid array that is unused or contain dead asteroid to overwrite*/
		while((AsteroidPtr->Object_Property.aliveFlag != RTE_ALIVE_FALSE)	&& (AsteroidPtr->Object_Property.aliveFlag != RTE_ALIVE_UNSET)){
			AsteroidPtr++;
			if(AsteroidPtr - Asteroid > (RTE_ASTEROID_BUFFER_SIZE-1)){
				return;
			}
		}

		vector_add(AsteroidVectPtr,AsteroidPtr);
		AsteroidPtr->Object_Property.aliveFlag = RTE_ALIVE_TRUE;
		AsteroidPtr->Object_Property.asteroidSize = RTE_ASTEROID_SIZE_M;
		AsteroidPtr->Object_Image.image = asteroid_medium_bmp;
		AsteroidPtr->Object_Image.imageWidth = RTE_ASTEROID_MEDIUM_BMP_W;
		AsteroidPtr->Object_Image.imageHeight = RTE_ASTEROID_MEDIUM_BMP_H;
		AsteroidPtr->Object_Image.clearWhenDead = RTE_DEAD_OBJECT_UNCLEARED;

		if(j == 0){
			AsteroidPtr->Object_Property.dx = RTE_random_sign()*2;
			AsteroidPtr->Object_Property.dy = RTE_random_sign()*2;
			AsteroidPtr->Object_Property.x = deadAsteroid_x + RTE_ASTEROID_MEDIUM_BMP_W;
			AsteroidPtr->Object_Property.y = deadAsteroid_y + RTE_ASTEROID_MEDIUM_BMP_H;
		}else if(j == 1){
			AsteroidPtr->Object_Property.dx = RTE_random_sign()*2;
			AsteroidPtr->Object_Property.dy = RTE_random_sign()*2;
			AsteroidPtr->Object_Property.x = deadAsteroid_x;
			AsteroidPtr->Object_Property.y = deadAsteroid_y;
		}
	}

	RNG_deinit();
}

/***********************************************************************
Private function: Display black background 
***********************************************************************/
void RTE_display_black_background(void)
{
	ILI9341_fill_display(ILI9341_BLACK);
}

/***********************************************************************
External function: Overwrite draw pixel function in ILI9341 driver library (in order to draw pixels going off screen)
***********************************************************************/
void ILI9341_draw_pixel (int16_t x, int16_t y, uint16_t color)
{
	RTE_wrap_cordinate(&x,&y);
	ILI9341_set_active_area(x,x,y,y);
	ILI9341_send_command(ILI9341_MEM_WRITE);
	ILI9341_send_parameter_16_bits(color);
}

/***********************************************************************
External function: Interrupt handler for TIM 6
***********************************************************************/
void TIM6_DAC_IRQHandler (void)
{
	TIM_intrpt_handler(TIM6);
	frameUpdate = SET;
}

void TIM3_IRQHandler (void)
{
	TIM_intrpt_handler(TIM3);
	shootButtonFirstTimeFlag = RTE_FIRST_TIME_TRUE;
}
