#include "ch.h"
#include "hal.h"

#include <main.h>
#include <dances.h>
#include <motors.h>

static uint8_t dance_status = 0;

static THD_WORKING_AREA(waDance2Rev, 1024);
static THD_FUNCTION(Dance2Rev, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;


    while(1){
    	dance_status = 0b1;
    }
}

void dance_start(void){
	chThdCreateStatic(waDance2Rev, sizeof(waDance2Rev), NORMALPRIO, Dance2Rev, NULL);
}

uint8_t is_dancing(void){
	return dance_status;
}
