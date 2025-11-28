/*
 * manual_fsm.c
 *
 *  Created on: Nov 26, 2024
 *      Author: tungn
 */

#include "manual_fsm.h"
#include "processing_fsm.h"
#include "output_led7seg.h"
#include "main.h"

enum FSM_STATE1 fsmStateMan = 0;
static uint8_t manualActive = 0;
static uint8_t blinkOn = 1;
static uint8_t confirmed = 0;
static int manualValue = 0;
static int prevRed = 0, prevAmber = 0, prevGreen = 0; // snapshot before adjustment chain
static uint8_t errorActive = 0; // TRUE while in error state

static int led7segMaxValue(void) {
    int maxv = 1;
    for (int i = 0; i < LED7SEG_DIGIT_NUMBER; i++) maxv *= 10;
    return maxv - 1;
}

int fsmManualIsActive(void) {
    return manualActive;
}

void fsmInitMan(void) {
    // Do not reset durations here
    fsmReInitMan(FSM_NORMAL_MAN);
}

void fsmReInitMan(enum FSM_STATE1 stateMan) {
    switch (stateMan) {
        case FSM_NORMAL_MAN:
            manualActive = 0;
            confirmed = 0;
            blinkOn = 1;
            led7segSetEnabled(1);
            fsmStateMan = FSM_NORMAL_MAN;
            break;

        case FSM_RED_MAN:
            manualActive = 1;
            confirmed = 0;
            blinkOn = 1;
            manualValue = trafficRedDuration;
            // store previous durations at start of adjustment chain
            prevRed = trafficRedDuration;
            prevAmber = trafficAmberDuration;
            prevGreen = trafficGreenDuration;
            led7segNumbers[0] = manualValue;
            led7segNumbers[1] = manualValue;
            trafficReInit(TRAFFIC_RED, 0);
            trafficReInit(TRAFFIC_RED, 1);
            led7segSetEnabled(1);
            timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
            fsmStateMan = FSM_RED_MAN;
            break;

        case FSM_AMBER_MAN:
            manualActive = 1;
            confirmed = 0;
            blinkOn = 1;
            manualValue = trafficAmberDuration;
            led7segNumbers[0] = manualValue;
            led7segNumbers[1] = manualValue;
            trafficReInit(TRAFFIC_AMBER, 0);
            trafficReInit(TRAFFIC_AMBER, 1);
            led7segSetEnabled(1);
            timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
            fsmStateMan = FSM_AMBER_MAN;
            break;

        case FSM_GREEN_MAN:
            manualActive = 1;
            confirmed = 0;
            blinkOn = 1;
            manualValue = trafficGreenDuration;
            led7segNumbers[0] = manualValue;
            led7segNumbers[1] = manualValue;
            trafficReInit(TRAFFIC_GREEN, 0);
            trafficReInit(TRAFFIC_GREEN, 1);
            led7segSetEnabled(1);
            timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
            fsmStateMan = FSM_GREEN_MAN;
            break;

        case FSM_ERROR_MAN:
            // Enter error state: start blink timers
            manualActive = 1;
            errorActive = 1;
            blinkOn = 1;
            led7segSetEnabled(1);
            timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1); // blink cadence
            timerSet(3000 / TIMER_DURATION, 2); // 3s duration
            fsmStateMan = FSM_ERROR_MAN;
            break;

        default:
            break;
    }
}

static int durationsAreValid(int r, int a, int g) {
    int maxv = 1; for (int i = 0; i < LED7SEG_DIGIT_NUMBER; i++) maxv *= 10; maxv -= 1;
    if (a < 1) return 0; // amber must be >= 1
    if (r < a) return 0; // red >= amber
    if (g < a) return 0; // green >= amber
    if (r > maxv || g > maxv || a > maxv) return 0;
    if (r != a + g) return 0; // red = amber + green
    return 1;
}

static void setAllTrafficOn(int on) {
    GPIO_PinState s = on ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(GPIOB, TRAFFIC0_RED_Pin, s);
    HAL_GPIO_WritePin(GPIOB, TRAFFIC0_AMBER_Pin, s);
    HAL_GPIO_WritePin(GPIOB, TRAFFIC0_GREEN_Pin, s);
    HAL_GPIO_WritePin(GPIOB, TRAFFIC1_RED_Pin, s);
    HAL_GPIO_WritePin(GPIOB, TRAFFIC1_AMBER_Pin, s);
    HAL_GPIO_WritePin(GPIOB, TRAFFIC1_GREEN_Pin, s);
}

void fsmManua(void) {
    switch (fsmStateMan) {
        case FSM_NORMAL_MAN:
            // Wait for main to gate or detect button0; keep idle here
            if (buttonPressed(0)) {
                fsmReInitMan(FSM_RED_MAN);
            }
            break;

        case FSM_RED_MAN:
            if (!confirmed && timerFlags[1] == 1) {
                timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                blinkOn = !blinkOn;
                if (blinkOn) {
                    led7segSetEnabled(1);
                    led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
                    trafficReInit(TRAFFIC_RED, 0);
                    trafficReInit(TRAFFIC_RED, 1);
                } else {
                    led7segSetEnabled(0);
                    trafficReInit(TRAFFIC_OFF, 0);
                    trafficReInit(TRAFFIC_OFF, 1);
                }
            }
            if (buttonPressed(1)) {
                manualValue++; if (manualValue > led7segMaxValue()) manualValue = 0;
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
            }
            if (buttonPressed(2)) {
                manualValue--; if (manualValue < 0) manualValue = led7segMaxValue();
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
            }
            if (buttonPressed(3)) {
                trafficRedDuration = manualValue;
                confirmed = 1;
                led7segSetEnabled(1);
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
                trafficReInit(TRAFFIC_RED, 0);
                trafficReInit(TRAFFIC_RED, 1);
            }
            if (confirmed && buttonPressed(0)) {
                fsmReInitMan(FSM_AMBER_MAN);
            }
            break;

        case FSM_AMBER_MAN:
            if (!confirmed && timerFlags[1] == 1) {
                timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                blinkOn = !blinkOn;
                if (blinkOn) {
                    led7segSetEnabled(1);
                    led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
                    trafficReInit(TRAFFIC_AMBER, 0);
                    trafficReInit(TRAFFIC_AMBER, 1);
                } else {
                    led7segSetEnabled(0);
                    trafficReInit(TRAFFIC_OFF, 0);
                    trafficReInit(TRAFFIC_OFF, 1);
                }
            }
            if (buttonPressed(1)) {
                manualValue++; if (manualValue > led7segMaxValue()) manualValue = 0;
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
            }
            if (buttonPressed(2)) {
                manualValue--; if (manualValue < 0) manualValue = led7segMaxValue();
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
            }
            if (buttonPressed(3)) {
                trafficAmberDuration = manualValue;
                confirmed = 1;
                led7segSetEnabled(1);
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
                trafficReInit(TRAFFIC_AMBER, 0);
                trafficReInit(TRAFFIC_AMBER, 1);
            }
            if (confirmed && buttonPressed(0)) {
                fsmReInitMan(FSM_GREEN_MAN);
            }
            break;

        case FSM_GREEN_MAN:
            if (!confirmed && timerFlags[1] == 1) {
                timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                blinkOn = !blinkOn;
                if (blinkOn) {
                    led7segSetEnabled(1);
                    led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
                    trafficReInit(TRAFFIC_GREEN, 0);
                    trafficReInit(TRAFFIC_GREEN, 1);
                } else {
                    led7segSetEnabled(0);
                    trafficReInit(TRAFFIC_OFF, 0);
                    trafficReInit(TRAFFIC_OFF, 1);
                }
            }
            if (buttonPressed(1)) {
                manualValue++; if (manualValue > led7segMaxValue()) manualValue = 0;
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
            }
            if (buttonPressed(2)) {
                manualValue--; if (manualValue < 0) manualValue = led7segMaxValue();
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
            }
            if (buttonPressed(3)) {
                trafficGreenDuration = manualValue;
                confirmed = 1;
                led7segSetEnabled(1);
                led7segNumbers[0] = manualValue; led7segNumbers[1] = manualValue;
                trafficReInit(TRAFFIC_GREEN, 0);
                trafficReInit(TRAFFIC_GREEN, 1);
            }
            if (confirmed && buttonPressed(0)) {
                if (!durationsAreValid(trafficRedDuration, trafficAmberDuration, trafficGreenDuration)) {
                    // Enter dedicated error state (keep current adjusted durations invalid until revert after blink)
                    fsmReInitMan(FSM_ERROR_MAN);
                } else {
                    manualActive = 0;
                    fsmReInitMan(FSM_NORMAL_MAN);
                    fsmReInit(FSM_NORMAL);
                }
            }
            break;

        case FSM_ERROR_MAN:
            // Blink all LEDs; revert after 3s using timer index 2
            if (timerFlags[1] == 1) {
                timerSet(TRAFFIC_BLINKING_DURATION / TIMER_DURATION, 1);
                blinkOn = !blinkOn;
                if (blinkOn) {
                    led7segSetEnabled(1);
                    led7segForceAllSegments(1); // force all segments ON
                    setAllTrafficOn(1);
                } else {
                    led7segForceAllSegments(0); // turn segments OFF
                    led7segSetEnabled(0);
                    setAllTrafficOn(0);
                }
            }
            if (timerFlags[2] == 1) {
                // Revert to previous valid durations and resume normal operation
                trafficRedDuration = prevRed;
                trafficAmberDuration = prevAmber;
                trafficGreenDuration = prevGreen;
                led7segForceAllSegments(0);
                led7segSetEnabled(1);
                setAllTrafficOn(0);
                errorActive = 0;
                manualActive = 0;
                fsmReInitMan(FSM_NORMAL_MAN);
                fsmReInit(FSM_NORMAL);
            }
            break;

        default:
            break;
    }
}


