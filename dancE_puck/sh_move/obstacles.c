/* obstacles.c
Principle of this file:
* Checks if obstacles. If:
* - no --> dance time
* - yes, everywhere --> alarm
* - yes, somewhere --> moves in the first found free direction
* If too much corrections of positions, the bot considers itself blocked
* When blocked, the bot controls sometimes if it can move again
* main function here: void avoid_obstacles()*/

#include "leds.h"
#include "motors.h"
#include <sensors/proximity.h>
#include "obstacles.h"

#include <stdbool.h>
#include <stdint.h>

//Sert seulement � tester compilation de fichier seul
/*void avoid_obstacles();
void move(int distance);
void alarm();			//if there are obstacles everywhere
void turn_right (int angle);
void turn_left (int angle);
uint8_t save_ir_dist();
int ctrl_if_no_more_obstacles();*/

#define MOVE_DEFAULT 500		//to change, distance by default when have to move
#define SPEED_DEFAULT 500		//steps/second, max 1000
#define LIMIT 1					//proximity limit for obstacles

uint8_t save_ir_dist(void){
	uint8_t all_prox = 0b0;
	for (uint8_t ir_nb = 0; ir_nb < 8; ir_nb++) {
		int val = get_prox(ir_nb);
		int intermediary = val > LIMIT;			//if high proximity
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
	return (save_ir_dist() == 0);
}

uint8_t correct_position_one_step(uint8_t all_prox){	//deleted , uint16_t nb_position_correct
	uint8_t has_corrected_pos = 0;
	switch (all_prox) {
		case 0 : 		//DANCE;
		break;
		
		case 0b11111111:	alarm();
		break; 
		
		case 0b11000000:	turn_left(45);
						move(MOVE_DEFAULT);
						has_corrected_pos += 1;
		break;
		
		case 0b00100000:	turn_left(90);
						move(MOVE_DEFAULT);
						has_corrected_pos += 1;
		break;
		
		case 0b00010000:	turn_left(135);
						move(MOVE_DEFAULT);
						has_corrected_pos += 1;
		break;
		
		case 0b00001000:	turn_right(135);
						move(MOVE_DEFAULT);
						has_corrected_pos += 1;
		break;
		
		case 0b00000100:	turn_right(90);
						move(MOVE_DEFAULT);
						has_corrected_pos += 1;
		break;
		
		case 0b00000011:	turn_right(45);
						move(MOVE_DEFAULT);
						has_corrected_pos += 1;
		break;
		default:		//plein de if;
			if ((all_prox | 11101100) == 11111100) {
				turn_right(45);
				move(MOVE_DEFAULT);
			} else if ((all_prox | 11101100) == 11101111) {
				turn_left(135);
				move(MOVE_DEFAULT);
			} else if ((all_prox | 00110111) == 00111111) {
				turn_left(45);
				move(MOVE_DEFAULT);
			} else if ((all_prox | 00110111) == 11110111) {
				turn_right(90);
				move(MOVE_DEFAULT);
			} else if ((all_prox | 11011011) == 11111011) {
				turn_right(90);
				move(MOVE_DEFAULT);
			} else if ((all_prox | 11011011) == 11011111) {
				turn_left(90);
				move(MOVE_DEFAULT);
			}
			has_corrected_pos += 1;
		break;
	}
	return has_corrected_pos;
}

void avoid_obstacles(void){
	//int limit = 1;				//limit of proximity useless because #defined
	uint16_t nb_position_correction_max = 1000;
	uint16_t nb_position_correct = 0;
	
	proximity_start();
	calibrate_ir();

	uint8_t all_prox = save_ir_dist();
	if (nb_position_correct < nb_position_correction_max) {
		nb_position_correct +=correct_position_one_step(all_prox);
	}
}

void move(uint16_t distance) {
	left_motor_set_pos(distance);
	right_motor_set_pos(distance);
	
	/* Variante si ne marche pas
	 * left_motor_set_speed(SPEED_DEFAULT);
	 * right_motor_set_speed(SPEED_DEFAULT);
	 * chfThdSleepMilliseconds(distance/SPEED_DEFAULT);
	 * left_motor_set_speed(0);
	 * right_motor_set_speed(0);
	 * */
}


void alarm(void){			//if there are obstacles everywhere
	uint8_t blocked = 1;
	while ((blocked == 1)) {
		set_led(1, 1);
		set_led(3, 1);
		set_led(5, 1);
		set_led(7, 1);
		chThdSleepMilliseconds(1000);	//sleeps 1000 ms
		set_led(1, 0);
		set_led(3, 0);
		set_led(5, 0);
		set_led(7, 0);
		if (ctrl_if_no_more_obstacles()) {blocked = 0;}
	}
}

void turn_right (uint16_t angle) {
	int32_t nb_steps = (3500/13)*angle;	//1000 steps/tour * wheel_diameter / perimeter
	left_motor_set_pos(-nb_steps);
	right_motor_set_pos(nb_steps);

	/* Variante si ne marche pas
	 * left_motor_set_speed(-SPEED_DEFAULT);
	 * righot_motor_set_speed(SPEED_DEFAULT);
	 * chfThdSleepMilliseconds(distance/SPEED_DEFAULT);
	 * left_motor_set_speed(0);
	 * righot_motor_set_speed(0);
	 * */
}

void turn_left (uint16_t angle){
	int32_t nb_steps = (3500/13)*angle;	//1000 steps/tour * wheel_diameter / perimeter
	left_motor_set_pos(nb_steps);
	right_motor_set_pos(-nb_steps);
	
	/* Variante si ne marche pas
	 * left_motor_set_speed(SPEED_DEFAULT);
	 * righot_motor_set_speed(-SPEED_DEFAULT);
	 * chfThdSleepMilliseconds(distance/SPEED_DEFAULT);
	 * left_motor_set_speed(0);
	 * righot_motor_set_speed(0);
	 * */
}