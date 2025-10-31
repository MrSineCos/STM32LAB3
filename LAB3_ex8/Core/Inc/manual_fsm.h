/*
 * manual_fms.h
 *
 *  Created on: Nov 26, 2024
 *      Author: tungn
 */

#ifndef INC_MANUAL_FSM_H_
#define INC_MANUAL_FSM_H_

#include "input_button.h"
#include "output_traffic.h"
#include "output_led7seg.h"

enum FSM_STATE1 {
    FSM_NORMAL_MAN,
    FSM_RED_MAN,
    FSM_AMBER_MAN,
    FSM_GREEN_MAN
};

void fsmInitMan(void);
void fsmReInitMan(enum FSM_STATE1 stateMan);
void fsmManua(void);


#endif /* INC_MANUAL_FSM_H_ */
