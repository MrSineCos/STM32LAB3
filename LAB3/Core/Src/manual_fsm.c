/*
 * manual_fsm.c
 *
 *  Created on: Nov 26, 2024
 *      Author: tungn
 */

#include "manual_fsm.h"
enum FSM_STATE1 fsmStateMan = 0;

void fsmInitMan(void) {
    trafficInit();
    fsmReInitMan(FSM_NORMAL_MAN);
}

void fsmReInitMan(enum FSM_STATE1 stateMan) {
    switch (stateMan) {
        case FSM_NORMAL_MAN:
            timerSet(TRAFFIC_SECOND_DURATION / TIMER_DURATION, 1);
            timerSet(trafficRedDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 2);
            timerSet(trafficGreenDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 3);
            led7segNumbers[0] = trafficRedDuration;
            led7segNumbers[1] = trafficGreenDuration;
            trafficReInit(TRAFFIC_RED, 0);
            trafficReInit(TRAFFIC_GREEN, 1);
            fsmStateMan = FSM_NORMAL_MAN;
            break;

        case FSM_RED_MAN:
            timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
            led7segNumbers[0] = 2;
            led7segNumbers[1] = trafficRedDuration;
            trafficReInit(TRAFFIC_OFF, 0);
            trafficReInit(TRAFFIC_OFF, 1);
            fsmStateMan = FSM_RED_MAN;
            break;

        case FSM_AMBER_MAN:
            timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
            led7segNumbers[0] = 3;
            led7segNumbers[1] = trafficAmberDuration;
            trafficReInit(TRAFFIC_OFF, 0);
            trafficReInit(TRAFFIC_OFF, 1);
            fsmStateMan = FSM_AMBER_MAN;
            break;

        case FSM_GREEN_MAN:
            timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
            led7segNumbers[0] = 4;
            led7segNumbers[1] = trafficGreenDuration;
            trafficReInit(TRAFFIC_OFF, 0);
            trafficReInit(TRAFFIC_OFF, 1);
            fsmStateMan = FSM_GREEN_MAN;
            break;

        default:
            break;
    }
}

void fsmManua(void) {
    switch (fsmStateMan) {
        case FSM_NORMAL_MAN:
            if (timerFlags[1] == 1) {
                timerSet(TRAFFIC_SECOND_DURATION / TIMER_DURATION, 1);
                led7segNumbers[0]--;
                if (led7segNumbers[0] < 0) led7segNumbers[0] = 0;
                led7segNumbers[1]--;
                if (led7segNumbers[1] < 0) led7segNumbers[1] = 0;
            }

            if (timerFlags[2] == 1) {
                switch (trafficState[0]) {
                    case TRAFFIC_RED:
                        timerSet(trafficGreenDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 2);
                        led7segNumbers[0] = trafficGreenDuration;
                        trafficReInit(TRAFFIC_GREEN, 0);
                        break;

                    case TRAFFIC_AMBER:
                        timerSet(trafficRedDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 2);
                        led7segNumbers[0] = trafficRedDuration;
                        trafficReInit(TRAFFIC_RED, 0);
                        break;

                    case TRAFFIC_GREEN:
                        timerSet(trafficAmberDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 2);
                        led7segNumbers[0] = trafficAmberDuration;
                        trafficReInit(TRAFFIC_AMBER, 0);
                        break;

                    default:
                        break;
                }
            }

            if (timerFlags[3] == 1) {
                switch (trafficState[1]) {
                    case TRAFFIC_RED:
                        timerSet(trafficGreenDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 3);
                        led7segNumbers[1] = trafficGreenDuration;
                        trafficReInit(TRAFFIC_GREEN, 1);
                        break;

                    case TRAFFIC_AMBER:
                        timerSet(trafficRedDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 3);
                        led7segNumbers[1] = trafficRedDuration;
                        trafficReInit(TRAFFIC_RED, 1);
                        break;

                    case TRAFFIC_GREEN:
                        timerSet(trafficAmberDuration * TRAFFIC_SECOND_DURATION / TIMER_DURATION, 3);
                        led7segNumbers[1] = trafficAmberDuration;
                        trafficReInit(TRAFFIC_AMBER, 1);
                        break;

                    default:
                        break;
                }
            }



        case FSM_RED_MAN:
            if (timerFlags[1] == 1) {
                switch (trafficState[0]) {
                    case TRAFFIC_OFF:
                        timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                        trafficReInit(TRAFFIC_RED, 0);
                        trafficReInit(TRAFFIC_RED, 1);
                        break;

                    case TRAFFIC_RED:
                        timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                        trafficReInit(TRAFFIC_OFF, 0);
                        trafficReInit(TRAFFIC_OFF, 1);
                        break;

                    default:
                        break;
                }
            }
            if (buttonPressed(0)) {
                    fsmReInitMan(FSM_AMBER_MAN);
                }

                if (buttonPressed(1)) {
                    led7segNumbers[1]++;
                    if (led7segNumbers[1] >= pow(10, LED7SEG_DIGIT_NUMBER)) {
                        led7segNumbers[1] = 0;
                    }
                }

                if (buttonPressed(2)) {
                    led7segNumbers[1]--;
                    if (led7segNumbers[1] < 0) {
                        led7segNumbers[1] = pow(10, LED7SEG_DIGIT_NUMBER) - 1;
                    }
                }

                if (buttonPressed(3)) {
                    trafficRedDuration = led7segNumbers[1];
                }
                break;

        case FSM_AMBER_MAN:
            if (timerFlags[1] == 1) {
                switch (trafficState[0]) {
                    case TRAFFIC_OFF:
                        timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                        trafficReInit(TRAFFIC_AMBER, 0);
                        trafficReInit(TRAFFIC_AMBER, 1);
                        break;

                    case TRAFFIC_AMBER:
                        timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                        trafficReInit(TRAFFIC_OFF, 0);
                        trafficReInit(TRAFFIC_OFF, 1);
                        break;

                    default:
                        break;
                }
            }

            if (buttonPressed(0)) {
                fsmReInitMan(FSM_GREEN_MAN);
            }
            break;

        case FSM_GREEN_MAN:
            if (timerFlags[1] == 1) {
                switch (trafficState[0]) {
                    case TRAFFIC_OFF:
                        timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                        trafficReInit(TRAFFIC_GREEN, 0);
                        trafficReInit(TRAFFIC_GREEN, 1);
                        break;

                    case TRAFFIC_GREEN:
                        timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                        trafficReInit(TRAFFIC_OFF, 0);
                        trafficReInit(TRAFFIC_OFF, 1);
                        break;

                    default:
                        break;
                }
            }
            if (buttonPressed(0)) {
                fsmReInitMan(FSM_NORMAL_MAN);
            }
            break;
        default:
            break;
    }
}


