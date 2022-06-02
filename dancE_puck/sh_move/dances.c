#include "dances.h"
#include "ch.h"
#include "hal.h"
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
#include <motor_control.h>


#define MS2STEP(MS) (((float)MS)/1000.f * SPEED_DEFAULT)

//local current dance variable
static dance_choice_t current_song = CORNER_GUY_D;

static uint8_t is_dancing = 0;

static BSEMAPHORE_DECL(danceProcess_sem, TRUE);

//tempos of the dance repertoire
// Pirates of the Caribbean main theme tempo
static const float pirate_tempo[] = {
  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 3, 10,

  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 3, 10,

  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 3, 10, 10,

  10, 10, 5, 10, 10,
  5, 10, 5, 10,
  10, 10, 5, 10, 10,
  10, 10, 3, 3,

  5, 10,
  //Rpeat of First Part
  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 3, 10,

  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 3, 10,

  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 10, 10, 10,

  10, 10, 5, 10, 10,
  5, 10, 10, 10,
  10, 10, 5, 10, 10,
  10, 10, 3, 3,
  //End of Repeat

  5, 10, 5, 5, 10, 5,
  10, 10, 10, 10, 10, 10, 10, 10, 5,
  5, 10, 5, 5, 10, 5,
  10, 10, 10, 10, 10, 2,

  5, 10, 5, 5, 10, 5,
  10, 10, 10, 10, 10, 10, 10, 10, 5,
  5, 10, 5, 5, 10, 5,
  10, 10, 10, 10, 10, 2,
};

//Sandstorms tempo
static const float sandstorms_tempo[] = {
  24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24,
  24, 24,
  24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24,
  24, 24,
  24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24,
};

//Mario main theme tempo
static const float mario_tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

//Star Wars tempo
static const float starwars_tempo[] = {
	3.6, 3.6, 3.6,
	4.8, 12, 3.6, 4.8, 12, 3.6,
	2,
	3.6, 3.6, 3.6,
	4.8, 12, 3.6, 4.8, 12, 3.6
};

static const uint16_t tempo_array_size[] = {
	0, 812, 248, 312, 76, 4
};


//------------------------------------INTERNAL FUNCTIONS-----------------------------------------

const float*  danceTable(dance_choice_t dnc){
	switch(dnc){
	case CORNER_GUY_D :
		return NULL;
	case PIRATES_OF_THE_CARIBBEAN_D :
		return pirate_tempo;
	case SANDSTORMS_D :
		return sandstorms_tempo;
	case MARIO_D :
		return mario_tempo;
	case STARWARS_D :
		return starwars_tempo;
	}
	return NULL;
}


void danceEX(dance_choice_t dnc){
	is_dancing = TRUE;
	uint16_t noteDuration = 0, pauseBetweenNotes = 0;
	const float* ref_d = danceTable(dnc);
	uint16_t len = tempo_array_size[dnc]/sizeof(ref_d[0]);
	for(uint16_t i = 0; i < len; i++){
		noteDuration = (uint16_t)(1000/ ref_d[i]);
		pauseBetweenNotes = (uint16_t)(noteDuration * 1.30);
		//move((int16_t)MS2STEP(noteDuration));
		dynamic_move(noteDuration);

		chThdSleepMilliseconds(pauseBetweenNotes);;
	}
	is_dancing = FALSE;
	chBSemSignal(&danceProcess_sem);

}


//-----------------------------------END INTERNAL FUNCTIONS--------------------------------------

void waitDanceFinish(void){
	chBSemWait(&danceProcess_sem);
}

//dance thread declaration
static THD_WORKING_AREA(waDanceThd, 2048);
static THD_FUNCTION(DanceThd, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    //systime_t time;

    while(1){
        //time = chVTGetSystemTime();
		chprintf((BaseSequentialStream*) &SD3,"dance loop proc\n\r" );

    	waitSoundPickUp();

    	chprintf((BaseSequentialStream*) &SD3,"dance wait done\n\r" );

    	switch(current_song){
    		case CORNER_GUY_D :


    			break;
    		case PIRATES_OF_THE_CARIBBEAN_D :
    			playMelody(PIRATES_OF_THE_CARIBBEAN, ML_SIMPLE_PLAY, NULL);

    			danceEX(current_song);
    			current_song = CORNER_GUY_D;
    			break;
    		case SANDSTORMS_D :
    			playMelody(SANDSTORMS, ML_SIMPLE_PLAY, NULL);

    			danceEX(current_song);
    			current_song = CORNER_GUY_D;
    			break;
    		case MARIO_D :
    			playMelody(MARIO, ML_SIMPLE_PLAY, NULL);

    			danceEX(current_song);
    			current_song = CORNER_GUY_D;
    			break;
    		case STARWARS_D :

    			playMelody(STARWARS, ML_FORCE_CHANGE, NULL);
    			danceEX(current_song);

    			current_song = CORNER_GUY_D;
    			break;
    	}


        //chThdSleepUntilWindowed(time, time + MS2ST(2000));
    	chThdSleepMilliseconds(1000);
    }

}


//------------------------------------EXTERNAL FUNCTIONS-----------------------------------------

uint8_t isDancing(void){
	return is_dancing;
}

void danceThd_start(void){
	current_song = CORNER_GUY_D;
	chThdCreateStatic(waDanceThd, sizeof(waDanceThd), NORMALPRIO+1, DanceThd, NULL);

}


void danceSetSong(dance_choice_t dnc){
	current_song = dnc;
}

dance_choice_t danceGetSong(void){
	return current_song;
}



//-----------------------------------END ExTERNAL FUNCTIONS--------------------------------------
