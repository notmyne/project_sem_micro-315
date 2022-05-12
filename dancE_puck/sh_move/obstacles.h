#ifndef OBSTACLES_H
#define OBSTACLES_H

/*#ifdef __cplusplus
extern "C" {
#endif*/

#define POSITION_NOT_REACHED 0
#define POSITION_REACHED 1
#define SPEED_DEFAULT 800		//steps/second, max 1000


void avoid_obstacles(void);
void move(int16_t distance); // distance must be given in steps
void obsAlarm(void);			//if there are obstacles everywhere
void turn_right (uint16_t angle);
void turn_left (uint16_t angle);
void obstacles_start(void);
uint8_t save_ir_dist(void);
uint8_t ctrl_if_no_more_obstacles(void);
uint8_t correct_position_one_step(uint8_t all_prox);

#endif
