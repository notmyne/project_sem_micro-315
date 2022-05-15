#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <ch.h>
#include <hal.h>


#define SPEED_DEFAULT 800		//steps/second, max 1100
#define MOVE_DEFAULT 500		//to change, distance by default when have to move
#define TURNING_COEFF 3.6179

void move(int16_t distance); // distance must be given in steps
void turn_right (uint16_t angle);
void turn_left (uint16_t angle);
void dynamic_move(uint16_t dms);//shuffles moves and dynmaically controls speed + distance



#endif

