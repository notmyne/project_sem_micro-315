#include "dances.h"
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>
#include <audio/play_melody.h>
#include <audio/audio_thread.h>
#include <obstacles.h>

//local current dance variable
static dance_choice_t current_song = CORNER_GUY_D;


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


//------------------------------------INTERNAL FUNCTIONS-----------------------------------------






//-----------------------------------END INTERNAL FUNCTIONS--------------------------------------


static THD_WORKING_AREA(waDanceThd, 512);
static THD_FUNCTION(DanceThd, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    //systime_t time;

    while(1){
        //time = chVTGetSystemTime();



        //chThdSleepUntilWindowed(time, time + MS2ST(2000));
    	chThdSleepMilliseconds(1000);
    }

}


//------------------------------------EXTERNAL FUNCTIONS-----------------------------------------
void danceThd_start(void){
	chThdCreateStatic(waDanceThd, sizeof(waDanceThd), NORMALPRIO, DanceThd, NULL);
}


void danceChangeSong(dance_choice_t dnc){
	current_song = dnc;
}

dance_choice_t danceGetSong(void){
	return current_song;
}


//-----------------------------------END ExTERNAL FUNCTIONS--------------------------------------
