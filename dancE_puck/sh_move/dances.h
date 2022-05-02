#ifndef DANCES_H
#define DANCES_H

typedef enum {
	DANCE_MARIO = 0,
	DANCE_PIRATES_OF_THE_CARIBBEAN,
	DANCE_STARWARS,
	DANCE_SANDSTORMS
}dance_selection_t;

//initialize the dance thread
void dance_start(void);
uint8_t is_dancing(void);


#endif
