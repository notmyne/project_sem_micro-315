/* obstacles.c
Principle of this file:
* Checks if obstacles. If:
* - no --> dance time
* - yes, everywhere --> obsAlarm
* - yes, somewhere --> moves in the first found free direction
* If too much corrections of positions, the bot considers itself blocked
* When blocked, the bot controls sometimes if it can move again
* main function here: void avoid_obstacles()*/

#include "leds.h"
#include "motors.h"
#include <sensors/proximity.h>
#include "obstacles.h"
#include <motor_control.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


//Sert seulement  à tester compilation de fichier seul
/*void avoid_obstacles(); void move(int distance); void alarm(); void turn_right (int angle); void turn_left (int angle); uint8_t save_ir_dist(); int ctrl_if_no_more_obstacles();*/



//------------------------OBSTACLES THREAD DECLARATION (mainly for IRs)--------------
static THD_WORKING_AREA(threadObstaclesWorkingArea, 128);

static THD_FUNCTION(threadObstacles, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    calibrate_ir();
    proximity_start();

	while(1) {
		avoid_obstacles();
		
		chThdSleepMilliseconds(500);
	}
}

void obstacles_start(void) {
	(void)chThdCreateStatic(threadObstaclesWorkingArea,
		sizeof(threadObstaclesWorkingArea), NORMALPRIO, threadObstacles, NULL);
	}

//------------------------OBSTACLES AVOIDANCE----------------------------------------
void avoid_obstacles(void){
	uint16_t nb_pos_corrections_max = 1000;
	uint16_t nb_pos_corrections = 0;

	uint8_t all_prox = save_ir_dist();

	if (nb_pos_corrections < nb_pos_corrections_max) {
		do {
			nb_pos_corrections +=correct_position_one_step(all_prox);
		} while (!ctrl_if_no_more_obstacles());
	} else {obsAlarm();}
}

uint8_t save_ir_dist(void){
	uint8_t all_prox = 0b0;
	for (uint8_t ir_nb = 0; ir_nb < 8; ir_nb++) {
		int val = get_prox(ir_nb);
		int intermediary = (val > LIMIT);	//if high proximity
		all_prox += intermediary << ir_nb;
	}
	if ((get_prox(0) > LIMIT) || (get_prox(1) > LIMIT)) {
		all_prox = all_prox | 11000000;
	}		//consider that Prox0 and Prox1 are nearly confounded
	if ((get_prox(6) > LIMIT) || (get_prox(7) > LIMIT)) {
		all_prox = all_prox | 00000011;
	}		//consider that Prox6 and Prox7 are nearly confounded
	return all_prox;
}

uint8_t ctrl_if_no_more_obstacles(void){
	return (save_ir_dist() == 0);				//return 1 if no obstacle
}

uint8_t correct_position_one_step(uint8_t all_prox){
	switch (all_prox) {
		case 0 : 			chThdSleepMilliseconds(500); //DANCE;
		break;
		
		case 0b11111111:	obsAlarm();			//obstacles everywhere
		break; 
		
		case 0b11000000:	turn_right(150);	//obstacle only in front left
							move(MOVE_DEFAULT);
							return 1;
		break;
		
		case 0b00100000:	turn_right(90);		//obstacle only in left
							move(MOVE_DEFAULT);
							return 1;
		break;
		
		case 0b00010000:	turn_right(30);		//obstacle only in back left
							move(MOVE_DEFAULT);
							return 1;
		break;
		
		case 0b00001000:	turn_left(30);		//obstacle only in back right
							move(MOVE_DEFAULT);
							return 1;
		break;
		
		case 0b00000100:	turn_left(90);		//obstacle only in right
							move(MOVE_DEFAULT);
							return 1;
		break;
		
		case 0b00000011:	turn_left(150);		//obstacle only in right
							move(MOVE_DEFAULT);
							return 1;
		break;
		
		default:								//otherwise: tries everything ccw
			if (!(all_prox & 0b11000000)) {
				turn_left(30);
				move(MOVE_DEFAULT);
				return 1;
			} else if (!(all_prox & 0b00100000)) {
				turn_left(90);
				move(MOVE_DEFAULT);
				return 1;
			} else if (!(all_prox & 0b00010000)) {
				turn_left(150);
				move(MOVE_DEFAULT);
				return 1;
			} else if (!(all_prox & 0b00001000)) {
				turn_right(150);
				move(MOVE_DEFAULT);
				return 1;
			} else if (!(all_prox & 0b00000100)) {
				turn_right(90);
				move(MOVE_DEFAULT);
				return 1;
			} else if (!(all_prox & 0b00000011)) {
				turn_right(30);
				move(MOVE_DEFAULT);
				return 1;
			}
			return 1;
		break;
	}
	return 0;
}


//-----------------------ALARM BLOCKING-----------------------

void obsAlarm(void){			//if there are obstacles everywhere
	do {
		set_led(0, 2);			//(led [not 1-7 --> all], value [not 0-1 --> toggle])
	} while (!(ctrl_if_no_more_obstacles()));
	set_led(0, 0);
}

//------------------------MOVES--------------------------------

//----------------------------------MOVE INTERNAL FUNCTIONS----------------------------------------

////xorshift 32bit RNG
//struct xorshift32_state {
//  uint32_t a;
//};
//
//static struct xorshift32_state rng_state = {2147483647};
//
//
///* The state word must be initialized to non-zero */
//uint32_t xorshift32(struct xorshift32_state *state)
//{
//	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
//	uint32_t x = state->a;
//	x ^= x << 13;
//	x ^= x >> 17;
//	x ^= x << 5;
//	return state->a = x;
//}



//-------------------------------END MOVE INTERNAL FUNCTIONS--------------------------------------

