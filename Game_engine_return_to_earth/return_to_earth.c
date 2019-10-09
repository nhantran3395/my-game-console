/**
*@brief test ili9341 driver library 's functions
*
*@author Tran Thanh Nhan
*@date 04/09/2019
*/

#include "stm32f407xx.h"                  // Device header
#include "game_engine.h"
#include <stdlib.h>
#include <stdio.h>

extern uint8_t frameUpdate;

extern Space_Object_t PlayerSpaceship;
extern Space_Object_t Asteroid[RTE_ASTEROID_BUFFER_SIZE] ;
extern Space_Object_t Rocket[RTE_ROCKET_BUFFER_SIZE];

extern vector AsteroidVect;
extern vector RocketVect;

extern uint8_t currentWave;
extern uint8_t numOfAsteroidInWave[RTE_NUM_OF_WAVE];

void delay(volatile uint32_t delay)
{
	for(;delay !=0;delay--);
}

int main (void)
{
	RTE_init();
	RTE_display_start_screen();
	while(SHOOT_BUTTON_READ);

	while(1){

		RTE_display_black_background();
		RTE_create_player_spaceship(&PlayerSpaceship);
		RTE_draw_player_spaceship(&PlayerSpaceship);

		RTE_create_asteroid(&AsteroidVect,Asteroid,numOfAsteroidInWave[currentWave],&PlayerSpaceship);
		RTE_draw_asteroid(&AsteroidVect);
		RNG_deinit();

		RTE_start_update_frame();

		while(1){

			if(frameUpdate == SET){

				RTE_display_score();

				RTE_update_player_spaceship(&PlayerSpaceship);
				RTE_draw_player_spaceship(&PlayerSpaceship);

				RTE_create_rocket(&RocketVect,Rocket,&PlayerSpaceship);
				RTE_update_rocket(&RocketVect,&AsteroidVect);
				RTE_draw_rocket(&RocketVect);

				RTE_update_asteroid(&AsteroidVect,&PlayerSpaceship);
				RTE_draw_asteroid(&AsteroidVect);

				if(PlayerSpaceship.Object_Property.aliveFlag == RTE_ALIVE_FALSE){
					PROTOBOARD_GREEN_LED_ON;
					RTE_display_game_over_screen();
					while(SHOOT_BUTTON_READ);
					RTE_reset_game();
					PROTOBOARD_GREEN_LED_OFF;
					break;
				}

				if(AsteroidVect.total == 0){
					TIM_ctr(TIM6,STOP);
					currentWave++;
					RNG_init();
					RTE_create_asteroid(&AsteroidVect,Asteroid,numOfAsteroidInWave[currentWave],&PlayerSpaceship);
					TIM_ctr(TIM6,START);
				}

				frameUpdate = CLEAR;
			}
		}
	}
}

void HardFault_Handler(void)
{
	PROTOBOARD_RED_LED_ON;
	while(1);
}
