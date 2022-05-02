#ifndef OBSTACLES_H
#define OBSTACLES_H

/*#ifdef __cplusplus
extern "C" {
#endif*/

void avoid_obstacles();
void move(int distance);
void alarm();			//if there are obstacles everywhere
void turn_right (int angle);
void turn_left (int angle);
uint8_t save_ir_dist();
int ctrl_if_no_more_obstacles();
