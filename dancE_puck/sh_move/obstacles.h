#ifndef OBSTACLES_H
#define OBSTACLES_H

/*#ifdef __cplusplus
extern "C" {
#endif*/

#include "ch.h"
#include "hal.h"



void avoid_obstacles(void);
void obsAlarm(void);			//if there are obstacles everywhere
void obstacles_start(void);
uint8_t save_ir_dist(void);
uint8_t ctrl_if_no_more_obstacles(void);
uint8_t correct_position_one_step(uint8_t all_prox);

#endif
