#include <motor_control.h>
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>
#include "leds.h"

#include <audio_processing.h>
#include <fft.h>
#include <arm_math.h>
#include <audio/play_melody.h>
#include <audio/audio_thread.h>
#include <obstacles.h>



//------------------------------------EXTERNAL FUNCTIONS-----------------------------------------

void move(int16_t distance) {	// distance must be given in steps
	int16_t position_to_reach_left  = distance, position_to_reach_right = distance;
	left_motor_set_pos(0);
	right_motor_set_pos(0);
	if (distance < 0){
		left_motor_set_speed(-SPEED_DEFAULT);
		right_motor_set_speed(-SPEED_DEFAULT);
		while(right_motor_get_pos() >= position_to_reach_right
		    && left_motor_get_pos() >= position_to_reach_left){
		}
	}else{
		left_motor_set_speed(SPEED_DEFAULT);
		right_motor_set_speed(SPEED_DEFAULT);
		while(right_motor_get_pos() <= position_to_reach_right
		    && left_motor_get_pos() <= position_to_reach_left){

		}
	}
	left_motor_set_speed(0);
	right_motor_set_speed(0);
//	position_to_reach_left  = 0;
//	position_to_reach_right = 0;
}

void turn_left (uint16_t angle) {
	int16_t pos_to_reach_right = TURNING_COEFF*angle, pos_to_reach_left = -pos_to_reach_right;

	left_motor_set_pos(0);
	right_motor_set_pos(0);
	left_motor_set_speed(-SPEED_DEFAULT);
	right_motor_set_speed(SPEED_DEFAULT);
	while(left_motor_get_pos() >= pos_to_reach_left
			&& right_motor_get_pos() <= pos_to_reach_right) {

	}
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

void turn_right (uint16_t angle){
	int16_t pos_to_reach_left = TURNING_COEFF*angle, pos_to_reach_right = -pos_to_reach_left;

	left_motor_set_pos(0);
	right_motor_set_pos(0);
	left_motor_set_speed(SPEED_DEFAULT);
	right_motor_set_speed(-SPEED_DEFAULT);
	while(left_motor_get_pos() <= pos_to_reach_left
			&& right_motor_get_pos() >= pos_to_reach_right) {

	}
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

void dynamic_move(uint16_t dms){
	//uint8_t danceIndex = (uint8_t)(xorshift32(&rng_state)%4);
	//chprintf((BaseSequentialStream*) &SD3, "dance index: %u\r\n", danceIndex);
	int16_t dynSpeed = 0;

//	//dynamic speed and distance control depending on duration
//	if(dms >255){ //linearly extrapolated exponential control
//		dynSpeed = (int16_t)(459.5-(0.77*dms));
//	}else{
//		dynSpeed = (int16_t)(947.65-(2.86*dms));
//	}
	dynSpeed = (int16_t)(1100-(2.15*dms));
	int16_t distance =  (int16_t)(0.001*(float)dms*dynSpeed);
	int16_t position_to_reach_left  = distance, position_to_reach_right = distance;

	left_motor_set_pos(0);
	right_motor_set_pos(0);
	if (distance < 0){
		left_motor_set_speed(-dynSpeed);
		right_motor_set_speed(-dynSpeed);
		while(right_motor_get_pos() >= position_to_reach_right
			&& left_motor_get_pos() >= position_to_reach_left){
		}
	}else{
		left_motor_set_speed(dynSpeed);
		right_motor_set_speed(dynSpeed);
		while(right_motor_get_pos() <= position_to_reach_right
			&& left_motor_get_pos() <= position_to_reach_left){

		}
	}
	left_motor_set_speed(0);
	right_motor_set_speed(0);

}
