/* obstacles.c
Principle of this file:
* Checks if obstacles. If:
* - no --> dance time
* - yes, everywhere --> blocked
* - yes, somewhere --> moves in the orthonormal direction from the obstacle
* If too much corrections of positions, the bot considers itself blocked
* When blocked, reset is required*/

#include "leds.h"
#include "motors.h"
#include <sensors/proximity.h>
#include "obstacles.h"
#include <motor_control.h>
#include <chprintf.h>
#include <msgbus/messagebus.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define LIMIT 150		//proximity limit for obstacles
#define CORRECTION_LIMIT 5

//Sert seulement  � tester compilation de fichier seul
/*void avoid_obstacles(); void move(int distance); void alarm(); void turn_right (int angle); void turn_left (int angle); uint8_t save_ir_dist(); int ctrl_if_no_more_obstacles();*/

//module wide bus, mutex & condvar
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

static const uint16_t ir_sensor_angles[] = {17, 47, 90, 120, 240, 270, 313, 343};


//--------------------------------------INTERNAL---------------------------------------
void evade_obstacle(uint8_t idx){
	if(idx < 4){
		turn_left(180-ir_sensor_angles[idx]);
		move(MOVE_DEFAULT);
	}else{
		turn_right(ir_sensor_angles[idx]-180);
		move(MOVE_DEFAULT);
	}
}


//------------------------OBSTACLES THREAD DECLARATION (mainly for IRs)--------------


static THD_WORKING_AREA(threadObstaclesWorkingArea, 2048);
static THD_FUNCTION(threadObstacles, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;


    uint8_t maxed = FALSE;
    uint16_t nb_pos_corrections = 0,  counter = 0;
    int16_t prox_vals[] ={0,0,0,0,0,0,0,0};

    messagebus_init(&bus, &bus_lock, &bus_condvar);
    proximity_start();

    calibrate_ir();

    while(1){

    	do{
			for(uint8_t i = 0; i < 8; i++){
				prox_vals[i]=get_prox(i);
				maxed = FALSE;
				if(prox_vals[i] >= LIMIT){
					maxed = TRUE;
					evade_obstacle(i);
					nb_pos_corrections++;
					break;
				}
			}
			if(nb_pos_corrections > CORRECTION_LIMIT){//dead loop in case not correctable, reset required

				left_motor_set_speed(0);
				right_motor_set_speed(0);

				do{
					counter++;
					if(counter == 1000){
						set_led(0, 2);			//(led [not 1-7 --> all], value [not 0-1 --> toggle])
						counter = 0;
					}
				}while (1);
			}
		}while(maxed);

    	maxed = FALSE;
    	nb_pos_corrections = 0;
		chThdSleepMilliseconds(500);
	}

}

void obstacles_start(void) {

	(void)chThdCreateStatic(threadObstaclesWorkingArea,
		sizeof(threadObstaclesWorkingArea), NORMALPRIO, threadObstacles, NULL);
}



