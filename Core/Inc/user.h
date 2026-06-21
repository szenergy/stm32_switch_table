/*
 * user.h
 *
 *	Made for Shell Eco Marathon by SZEnergy team
 *
 *  Created on: Mar 5, 2026
 *      Author: varma02@Github
 */

#ifndef INC_USER_H_
#define INC_USER_H_

#include "main.h"

// *** Definitions ***

// Optimal lap time in seconds, used for strategy
#define OPTIMAL_LAP (float)191.1

// These variables are used for lap button protection
// Set the minimum time and distance required for the lap button to register again after a press
#define LAP_BTN_MIN_TIME (float)5
#define LAP_BTN_MIN_DISTANCE (float)50

// Throttle pedal limits
#define POT_ZERO   (float)2150
#define POT_MAX    (float)2350

// This number is multiplied by the wheel RPM to get the vehicle speed in km/h
#define SPEED_MULT_FACTOR (float)0.10376

// Rate limits for changes in torque per 50 milliseconds
#define RATE_LIMIT_UP   (float)0.05
#define RATE_LIMIT_DOWN (float)0.25

// The time between wipes is in ticks, each tick is equal to 50 milliseconds
#define WIPER_PERIOD   (uint16_t)25     // 25 => 1.25s
// Configure the wiper positions
// The current servo is an H-SPEED HSX181, its PWM region
// starts at 0.5ms = 1000 counts, and ends at 2.8ms = 5500 counts at 50Hz
#define WIPER_LEFT     (uint16_t)4600
#define WIPER_RIGHT    (uint16_t)4000
#define WIPER_CENTER   (uint16_t)4100

// Set the time between BMS query messages in ticks
// 1 tick is equal to 50 milliseconds
#define BMS_QUERY_PERIOD (uint16_t)40   // 40 => 2s

// The following are debug options.
// Uncomment (remove the // from the start) the defines to enable them.
//#define DEBUG_LEDS            // Debug LEDs on the panel's side
//#define SLEEP_DEBUG_PIN       // Output sleep/awake state on PC12 pin
//#define WIPER_SWEEP_DEBUG     // Moves wiper PWM from 0% fill to 100% slowly
//#define UART_DEBUG            // Sends some debug values on the UART


// *** Global variables ***


extern uint16_t throttle_adc_buffer[10];


// *** User Functions ***


void User_Init(CAN_HandleTypeDef *can_ptr, TIM_HandleTypeDef *wiper_pwm_ptr, UART_HandleTypeDef *uart_ptr);
void User_Loop();
void Debug_Msg(char *key, uint32_t value);


// *** Error handling ***


typedef enum {
	UERR_UNKNOWN = 1,
	UERR_CAN_FILTER_CONFIG,
	UERR_CAN_START,
	UERR_CAN_MSG_SEND,
	UERR_CAN_RECEIVE_STW,
	UERR_CAN_RECEIVE_ENCODER,
	UERR_WAKE_TIMER_FAIL,
	UERR_ADC_START_FAIL,
	UERR_ADC_TIMER_FAIL
} USER_ERROR;
void User_Error_Handler(USER_ERROR err, uint8_t fatal);


// *** Data structures and types ***


typedef enum {
	DM_NEUTRAL,
	DM_MANUAL,
	DM_MANUAL_STRATEGY,
	DM_AUTOMATIC_STRATEGY,
	DM_SPEED_HOLD
} DRIVE_MODE_ENUM;

typedef union  __attribute__(())
{
    uint8_t bits;
    struct
    {
        uint8_t LIGHTS_DRL      :1;
        uint8_t LIGHTS_HAZARD   :1;
        uint8_t AUTONOMOUS      :1;
        uint8_t BRAKE           :1;
        uint8_t LIGHTS_ENABLE   :1;
        uint8_t MC_OW           :1;
        uint8_t WIPER           :1;
        uint8_t PESC_SLEEP      :1;
    };
} VCU_STATE_A;

typedef union __attribute__(())
{
    uint8_t bits;
    struct
    {
        uint8_t RELAY_NO        :1;
        uint8_t RELAY_NC        :1;
        uint8_t RESERVED1       :1;
        uint8_t RESERVED2       :1;
        uint8_t RESERVED3       :1;
        uint8_t RESERVED4       :1;
        uint8_t RESERVED5       :1;
        uint8_t RESERVED6       :1;
    };
} VCU_STATE_B;

typedef union  __attribute__(())
{
    uint8_t bits;
    struct
    {
        uint8_t ACC        :1;
        uint8_t DRIVE      :1;
        uint8_t REVERSE    :1;
        uint8_t LAP        :1;
        uint8_t TS_L       :1;
        uint8_t TS_R       :1;
        uint8_t FUNCTION1  :1;
        uint8_t FUNCTION2  :1;
    };
} STW_STATE_BUTTONS;

typedef enum {
	ROT_1 = 0,
	ROT_2 = 1,
	ROT_3 = 2,
	ROT_4 = 4,
	ROT_5 = 8,
	ROT_6 = 16,
	ROT_7 = 32,
	ROT_8 = 64
} ROT_POS_ENUM;

#define ROT_TO_INT (uint8_t[]){[ROT_1] = 1, [ROT_2] = 2, [ROT_3] = 3, [ROT_4] = 4, [ROT_5] = 5, [ROT_6] = 6, [ROT_7] = 7, [ROT_8] = 8}

#endif /* INC_USER_H_ */
