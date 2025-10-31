/*
 * software_timer.h
 *
 *  Created on: Nov 26, 2024
 *      Author: tungn
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"

#define TIMER_NUMBER 4
#define TIMER_DURATION 10

extern uint8_t timerFlags[TIMER_NUMBER];

void timerSet(int duration, int index);
void timerRun(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
