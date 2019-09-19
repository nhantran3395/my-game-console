/**
*@brief test ili9341 driver library 's functions
*
*@author Tran Thanh Nhan
*@date 04/09/2019
*/

#include "stm32f407xx.h"                  // Device header
#include "game_engine.h"
#include "../Peripheral_drivers/inc/stm32f407xx_uart.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern uint8_t frameUpdate;
extern uint8_t numOfAsteroid;

Space_Object_t Asteroid[NUM_OF_ASTEROID] ;
Space_Object_t PlayerSpaceship;
Space_Object_t Rocket[NUM_OF_ROCKET] ;

void delay(volatile uint32_t delay)
{
	for(;delay !=0;delay--);
}

int main (void)
{
	led_init(GPIOD,GPIO_PIN_NO_13);
	led_init(GPIOD,GPIO_PIN_NO_15);

	RTE_init();	
		
	RTE_display_start_screen();
	while(SHOOT_BUTTON_READ);
	
	while(1){

		RTE_display_black_background();
		RTE_create_player_spaceship(&PlayerSpaceship);
		RTE_draw_player_spaceship(&PlayerSpaceship);

		RTE_create_asteroid(Asteroid,numOfAsteroid,&PlayerSpaceship);
		RTE_draw_asteroid(Asteroid,numOfAsteroid);
		RNG_deinit();

		RTE_start_update_frame();

		while(1){

			if(frameUpdate == SET){

				RTE_display_score();

				RTE_update_player_spaceship(&PlayerSpaceship);
				RTE_draw_player_spaceship(&PlayerSpaceship);

				RTE_create_rocket(Rocket,&PlayerSpaceship);
				RTE_update_rocket(Rocket);
				RTE_draw_rocket(Rocket);

				RTE_update_asteroid(Asteroid,numOfAsteroid,&PlayerSpaceship,Rocket);
				RTE_draw_asteroid(Asteroid,numOfAsteroid);

				if(PlayerSpaceship.Object_Property.aliveFlag == RTE_ALIVE_FALSE){
					led_on(GPIOD,GPIO_PIN_NO_13);
					RTE_display_game_over_screen();
					while(SHOOT_BUTTON_READ);
					RTE_reset_game();
					break;
				}
	
				frameUpdate = CLEAR;
			}
		}
	}
}

void HardFault_Handler(void)
{
	led_on(GPIOD,GPIO_PIN_NO_15);
	while(1);
}
