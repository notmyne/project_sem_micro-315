#ifndef OBSTACLES_H
#define OBSTACLES_H

/*#ifdef __cplusplus
extern "C" {
#endif*/

void avoid_obstacles(void);
void move(uint16_t distance);
void alarm(void);			//if there are obstacles everywhere
void turn_right (uint16_t angle);
void turn_left (uint16_t angle);
uint8_t save_ir_dist(void);
uint8_t ctrl_if_no_more_obstacles(void);
uint8_t correct_position_one_step(uint8_t all_prox);

#endif
