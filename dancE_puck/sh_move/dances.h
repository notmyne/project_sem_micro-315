#ifndef DANCES_H
#define DANCES_H

#include <ch.h>
#include <hal.h>

typedef enum{
	CORNER_GUY_D = 0,
	PIRATES_OF_THE_CARIBBEAN_D,
	SANDSTORMS_D,
	MARIO_D,
	STARWARS_D

}dance_choice_t;


uint8_t isDancing(void);
void waitDanceFinish(void);

void danceThd_start(void);
void danceSetSong(dance_choice_t dnc);
dance_choice_t danceGetSong(void);

#endif
