/*
 * user.c
 *
 *	Made for Shell Eco Marathon by SZEnergy team
 *
 *  Created on: Mar 5, 2026
 *      Author: varma02@Github
 */

#include "user.h"
#ifdef UART_DEBUG
#include <stdio.h>
#endif

struct {
	VCU_STATE_A A;
	VCU_STATE_B B;
} vcu_state;

struct {
	STW_STATE_A A;
	STW_STATE_B B;
	STW_STATE_C C;
	STW_STATE_D D;
} stw_state;

double wheel_rpm;

struct {
	DRIVE_STATE prev;
	DRIVE_STATE current;
} drive_state;

uint16_t throttle_adc_buffer[10];
struct {
	uint16_t current;
	uint16_t prev;
} throttle_value;

uint8_t wiper_ARR = 0;
struct WIPER_STATE wiper_state;

struct {
	uint16_t current;
	uint16_t prev;
} rate_limiter;

CAN_HandleTypeDef *_usr_can;
TIM_HandleTypeDef *_usr_wiper_pwm;
UART_HandleTypeDef *_usr_uart;

/**
 * This function sends the key-value pair over UART for logging or debug.
 * MAX MESSAGE LENGTH IS 128.
 * @param key - variable/information title (eg. "SOMEVALUE")
 * @param value - the number value information
 */
#ifdef UART_DEBUG
void Debug_Msg(char *key, uint32_t value) {
	char msg[128];
	uint16_t len = sprintf(msg, "%s = %d\r\n", key, value);
	HAL_UART_Transmit(_usr_uart, (uint8_t*)msg, len, 100);
}
#endif


/**
 * User side error handling function.
 * For non-critical errors it just flashes the Yellow LED and logs on UART (if the ERROR_REPORTING flag is defined).
 * If the error is fatal and the program cannot continue, it turns on the Red LED and goes into an infinite loop.
 * @param err - a value from the USER_ERROR enum (eg. UERR_CAN_START)
 * @param fatal - if the program can continue after the error or not
 */
void User_Error_Handler(USER_ERROR err, uint8_t fatal) {
#ifdef UART_DEBUG
	Debug_Msg("ERROR", err);
	Debug_Msg("FATAL", fatal);
#endif
	if (fatal == SET) {
		HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_Red_GPIO_Port, LED2_Red_Pin, GPIO_PIN_SET);
		__disable_irq();
		while (1) {
			HAL_GPIO_TogglePin(LED2_Red_GPIO_Port, LED2_Red_Pin);
			HAL_Delay(250);
		}
	} else {
#ifdef DEBUG_LEDS
		HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_SET);
#endif
	}
}

/**
 * Rate limiter function for the motor torque reference.
 * Can be configured in the `user.h` with RATE_LIMIT_UP and RATE_LIMIT_DOWN.
 * @param value - desired value
 * @return - rate limited value
 */
uint16_t _Rate_Limit(uint16_t value) {
	rate_limiter.prev = rate_limiter.current;
	rate_limiter.current = value;
    if(2000 + rate_limiter.current - rate_limiter.prev > 2000 + RATE_LIMIT_UP)
        rate_limiter.current = rate_limiter.prev + RATE_LIMIT_UP;
    else if(2000 + rate_limiter.current - rate_limiter.prev < 2000 - RATE_LIMIT_DOWN)
        rate_limiter.current = rate_limiter.prev - RATE_LIMIT_DOWN;
    return rate_limiter.current;
}

/**
 * Averages the last 10 values provided by the ADC through DMA.
 * @return - the averaged value
 */
uint16_t _ADC_Moving_Average() {
	uint32_t sum = 0;
	for (uint8_t i = 0; i < 10; i++){
		sum += throttle_adc_buffer[i];
#ifdef UART_DEBUG
		char key[32];
		sprintf(key, "POT_ADC_%d", i);
		Debug_Msg(key, throttle_adc_buffer[i]);
#endif
	}
	return sum / 10;
}

/**
 * Reads, filters and debounces the current value of the potentiometer
 * Can be configured from `user.h` with the POT_* variables.
 */
void _Pot_Filter() {
	throttle_value.prev = throttle_value.current;
	throttle_value.current = _ADC_Moving_Average();

#ifdef UART_DEBUG
	Debug_Msg("POT_ADC_AVG", throttle_value.current);
#endif

	if ((throttle_value.current - throttle_value.prev) <= POT_STEP)
		throttle_value.current = throttle_value.prev;

	if (throttle_value.current <= POT_ZERO + POT_STEP) {
		throttle_value.current = POT_VALUES[0];
	} else if (throttle_value.current <= POT_ZERO + POT_STEP * 19) {
		throttle_value.current = POT_VALUES[(throttle_value.current - POT_ZERO) / POT_STEP];
	} else {
		throttle_value.current = POT_VALUES[19];
	}

#ifdef UART_DEBUG
	Debug_Msg("POT_ADC_FLTR", throttle_value.current);
#endif
}

/**
 * Updates the drive state machine based on the current switch positions.
 */
void _Update_Drive_State() {
#ifdef UART_DEBUG
	Debug_Msg("PREV_STATE", drive_state.current);
#endif
	drive_state.prev = drive_state.current;
	if (vcu_state.A.MC_OW == RESET) {
		if (stw_state.A.DRIVE == RESET && stw_state.A.REVERSE == RESET) {
			drive_state.current = D_NEUTRAL;
		} else if (throttle_value.current > 0) {
			if (stw_state.A.DRIVE == SET) {
				drive_state.current = D_DRIVE_PEDAL;
			} else if (stw_state.A.REVERSE == SET) {
				drive_state.current = D_REVERSE_PEDAL;
			}
		} else if (stw_state.A.ACC == SET) {
			if (stw_state.A.DRIVE == SET) {
				drive_state.current = D_AUTO_ACC;
			} else if (stw_state.A.REVERSE == SET) {
				drive_state.current = D_AUTO_DEC;
			}
		} else {
			drive_state.current = D_NEUTRAL;
		}
	} else {
		drive_state.current = D_NEUTRAL;
	}
#ifdef UART_DEBUG
	Debug_Msg("CURR_STATE", drive_state.current);
#endif
}

/**
 * Calculates the torque reference value to send to the motor controller.
 * Uses the lookup tables from `user.h` in certain switch positions.
 * @return - unsigned integer value
 */
uint16_t _Calculate_MC_Ref() {
	if (vcu_state.A.BRAKE != RESET)
		return 0; // the brake pedal should inhibit acceleration

	uint16_t reference = throttle_value.current;

	if (drive_state.current != D_NEUTRAL) {
		if (drive_state.current == D_AUTO_ACC || drive_state.current == D_AUTO_DEC) {
			uint16_t speed = wheel_rpm * SPEED_MULT_FACTOR;
			switch (stw_state.D.bits) {
				case 0:
				case 1:
				case 8:
					reference = 1023;
					break;
				case 2:
					if (wheel_rpm >= 116)
						reference = 1023 * LUT_Z22[speed];
					else
						reference = 1023;
					break;
				case 4:
					if (wheel_rpm >= 221)
						reference = 1023 * LUT_Z24[speed];
					else
						reference = 1023;
					break;
				case 16:
					if (speed >= 5)
						reference = 409;
					else
						reference = 1023;
					break;
				case 32:
					reference = 818;
					break;
				case 64:
					reference = 920;
					break;
				default:
					reference = 0;
					break;
			}
		}
	} else if (drive_state.prev != D_NEUTRAL && drive_state.current == D_NEUTRAL) {
		rate_limiter.current = 0;
		rate_limiter.prev = 0;
		reference = 0;
	} else {
		reference = 0;
	}

#ifdef UART_DEBUG
	Debug_Msg("MC_REF", drive_state.current);
#endif

	return _Rate_Limit(reference);
}

/**
 * Reads the GPIO pins.  ¯\_(ツ)_/¯
 */
void _Read_GPIO_Inputs() {
	vcu_state.A.AUTONOMOUS     = HAL_GPIO_ReadPin(Autonomous_switch_GPIO_Port, Autonomous_switch_Pin);
	vcu_state.A.HAZARD         = HAL_GPIO_ReadPin(Hazard_switch_GPIO_Port, Hazard_switch_Pin);
	vcu_state.A.LIGHTS_ENABLE  = HAL_GPIO_ReadPin(Lights_enable_switch_GPIO_Port, Lights_enable_switch_Pin);
	vcu_state.A.MC_OW          = HAL_GPIO_ReadPin(Motorcontrol_override_switch_GPIO_Port, Motorcontrol_override_switch_Pin);
	vcu_state.A.WIPER          = HAL_GPIO_ReadPin(Wiper_switch_GPIO_Port, Wiper_switch_Pin);
	vcu_state.A.HEADLIGHT      = HAL_GPIO_ReadPin(Headlight_switch_GPIO_Port, Headlight_switch_Pin);
	vcu_state.A.BRAKE          = HAL_GPIO_ReadPin(Brake_pedal_input_GPIO_Port, Brake_pedal_input_Pin);
}

/**
 * Initializes the CAN filters for the two processed messages,
 * 0x190 which is from the steering wheel and contains button and switch positions
 * and 0x123 which contains the current wheel RPM from the encoder.
 * The first one goes to FIFO0, the second one to FIFO1.
 */
void _Init_CAN_Filters() {
	CAN_FilterTypeDef filter_0x190;

	filter_0x190.FilterBank =           14;
	filter_0x190.SlaveStartFilterBank = 14;
	filter_0x190.FilterMode =           CAN_FILTERMODE_IDLIST;
	filter_0x190.FilterScale =          CAN_FILTERSCALE_16BIT;
	filter_0x190.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter_0x190.FilterActivation =     CAN_FILTER_ENABLE;

	filter_0x190.FilterIdHigh =     (0x190 << 5);
	filter_0x190.FilterIdLow =      (0x190 << 5);
	filter_0x190.FilterMaskIdHigh = (0x190 << 5);
	filter_0x190.FilterMaskIdLow =  (0x190 << 5);

	if (HAL_CAN_ConfigFilter(_usr_can, &filter_0x190) != HAL_OK) {
		User_Error_Handler(UERR_CAN_FILTER_CONFIG, 1);
	}

	CAN_FilterTypeDef filter_0x123;

	filter_0x123.FilterBank =           15;
	filter_0x123.SlaveStartFilterBank = 14;
	filter_0x123.FilterMode =           CAN_FILTERMODE_IDLIST;
	filter_0x123.FilterScale =          CAN_FILTERSCALE_16BIT;
	filter_0x123.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	filter_0x123.FilterActivation =     CAN_FILTER_ENABLE;

	filter_0x123.FilterIdHigh =     (0x123 << 5);
	filter_0x123.FilterIdLow =      (0x123 << 5);
	filter_0x123.FilterMaskIdHigh = (0x123 << 5);
	filter_0x123.FilterMaskIdLow =  (0x123 << 5);

	if (HAL_CAN_ConfigFilter(_usr_can, &filter_0x123) != HAL_OK) {
		User_Error_Handler(UERR_CAN_FILTER_CONFIG, 1);
	}
}

/**
 * Initializes the CAN2 peripheral and starts it.
 */
void _Init_CAN() {
	_Init_CAN_Filters();

	if (HAL_CAN_Start(_usr_can) != HAL_OK)
	{
		User_Error_Handler(UERR_CAN_START, 1);
	}
}

/**
 * Processes the received CAN messages and stores the values in global variables.
 * stw_state - the inputs from the steering wheel
 * wheel_rpm - the current RPM value from the encoder
 */
void _Receive_CAN() {
	CAN_RxHeaderTypeDef header;
	uint8_t data[8];

	if (HAL_CAN_GetRxMessage(_usr_can, CAN_RX_FIFO0, &header, data) == HAL_OK) {
		stw_state.A.bits = data[0];
		stw_state.B.bits = data[1];
		stw_state.C.bits = data[2];
		stw_state.D.bits = data[3];
	} else {
		User_Error_Handler(UERR_CAN_RECEIVE, 0);
	}

	if (HAL_CAN_GetRxMessage(_usr_can, CAN_RX_FIFO1, &header, data) == HAL_OK) {
		wheel_rpm = ((uint16_t)data[0] << 8) | data[1];
	} else {
		User_Error_Handler(UERR_CAN_RECEIVE, 0);
	}
}

/**
 * Prepares, packs and sends the VCU_State CAN frame.
 * This contains the switch positions and the current filtered ADC input.
 */
void _Send_VCU_State_CAN() {
	CAN_TxHeaderTypeDef header;
	uint8_t data[6];
	uint32_t mailbox;

	header.IDE = CAN_ID_STD;
	header.StdId = 0x129;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 6;

	data[0] = vcu_state.A.bits;
	data[1] = vcu_state.B.bits;

	int32_t throttle_buffer = (throttle_value.current * 100000) / 1023;
	data[2] = throttle_buffer >> 24;
	data[3] = throttle_buffer >> 16;
	data[4] = throttle_buffer >> 8;
	data[5] = throttle_buffer;

	if (HAL_CAN_AddTxMessage(_usr_can, &header, data, &mailbox) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
		return;
	}
#ifdef DEBUG_LEDS
	HAL_GPIO_TogglePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin);
#endif
}

/**
 * Sends the torque reference to the motor controller over CAN.
 * @param reference - the value to send
 */
void _Send_MC_Command_CAN(uint16_t reference) {
	CAN_TxHeaderTypeDef header;
	uint8_t data[4];
	uint32_t mailbox;

	header.IDE = CAN_ID_EXT;
	header.ExtId = 0xA51;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 4;

	int32_t throttle_buffer = (reference * 100000) / 1023;
	if (stw_state.A.REVERSE)
		throttle_buffer = throttle_buffer * -1;
	data[0] = throttle_buffer >> 24;
	data[1] = throttle_buffer >> 16;
	data[2] = throttle_buffer >> 8;
	data[3] = throttle_buffer;

	if (HAL_CAN_AddTxMessage(_usr_can, &header, data, &mailbox) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
	}
}

/**
 * This is the periodic wiper.
 * It checks for the wiper switch to be turned on, then executes the left <-> right motion.
 * This also turns on/off the wiper DC-DC converter for the servo.
 * The wiper positions and oscillation frequency are configurable from `user.h`.
 */
void _Wiper_Tick() {
	switch (wiper_state.step) {
	case 0: // Standby state, waiting for wiper switch signal
		if (vcu_state.A.WIPER == SET)
			wiper_state.step = 1;
		break;
	case 1: // Setup phase, start PWM and converter
		HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_SET);
		HAL_TIM_PWM_Start(_usr_wiper_pwm, TIM_CHANNEL_1);
		wiper_state.running = SET;
		wiper_state.step = 2;
		break;
#ifdef WIPER_SWEEP_DEBUG
	case 2:
		for (uint16_t i = 0; i < _usr_wiper_pwm->Init.Period-20; i+=20){
			if (HAL_GPIO_ReadPin(Wiper_switch_GPIO_Port, Wiper_switch_Pin) == GPIO_PIN_RESET) break;
			_usr_wiper_pwm->Instance->CCR1 = i;
#ifdef UART_DEBUG
			Debug_Msg("WIPER_CCR", i);
#endif
			HAL_Delay(20);
		}
		wiper_state.step = 5;
#else
	case 2: // Wipe Right
		if (vcu_state.A.WIPER == RESET) {
			wiper_state.step = 4;
		} else {
			_usr_wiper_pwm->Instance->CCR1 = WIPER_RIGHT;
			wiper_state.step = 3;
		}
		break;
	case 3: // Wipe Left
		if (vcu_state.A.WIPER == RESET) {
			wiper_state.step = 4;
		} else {
			_usr_wiper_pwm->Instance->CCR1 = WIPER_LEFT;
			wiper_state.step = 2;
		}
		break;
	case 4: // Switch is off, go to the center
		_usr_wiper_pwm->Instance->CCR1 = WIPER_CENTER;
		wiper_state.step = 5;
		break;
#endif
	case 5: // Turn off and go to standby
		HAL_TIM_PWM_Stop(_usr_wiper_pwm, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_RESET);
		wiper_state.running = RESET;
		wiper_state.step = 0;
		break;
	default:
		break;
	}
#ifdef UART_DEBUG
	Debug_Msg("WIPER_RUN", wiper_state.running);
	Debug_Msg("WIPER_STEP", wiper_state.step);
#endif
}

/**
 * Resets all the GPIO output pins to off.
 */
void _Reset_Outputs() {
	HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_Red_GPIO_Port, LED2_Red_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Brake_light_GPIO_Port, Brake_light_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Debug_Out_GPIO_Port, Debug_Out_Pin, GPIO_PIN_RESET);
}

/**
 * Resets the user defined variables to their defaults.
 */
void _Reset_Variables() {
	vcu_state.A.bits = 0b00000000;
	vcu_state.B.bits = 0b00000000;
	stw_state.A.bits = 0b00000000;
	stw_state.B.bits = 0b00000000;
	stw_state.C.bits = 0b00000000;
	stw_state.D.bits = 0b00000000;
	wheel_rpm = 0;
	drive_state.current = D_NEUTRAL;
	drive_state.prev = D_NEUTRAL;
	for (uint8_t i = 0; i < 10; i++) {
		throttle_adc_buffer[i] = 0;
	}
	throttle_value.current = POT_ZERO;
	throttle_value.prev = POT_ZERO;
	rate_limiter.current = 0;
	rate_limiter.prev = 0;
	wiper_state.running = RESET;
	wiper_state.step = 0;
	wiper_ARR = 0;
}

/**
 * Initializes CAN and variables, as well as resetting every GPIO output.
 */
void User_Init(CAN_HandleTypeDef *can_ptr, TIM_HandleTypeDef *wiper_pwm_ptr, UART_HandleTypeDef *uart_ptr) {
	_Reset_Variables();

	_usr_can = can_ptr;
	_usr_wiper_pwm = wiper_pwm_ptr;
	_usr_uart = uart_ptr;

	_Reset_Outputs();
	_Init_CAN();

#ifdef DEBUG_LEDS
	HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_SET);
#endif
}

/**
 * Executes one tick.
 */
void User_Loop() {
	_Read_GPIO_Inputs();

	HAL_GPIO_WritePin(Brake_light_GPIO_Port, Brake_light_Pin, vcu_state.A.BRAKE);

	_Receive_CAN();

	_Pot_Filter();

	_Send_VCU_State_CAN();

	if (vcu_state.A.MC_OW == RESET && vcu_state.A.AUTONOMOUS == RESET) {
		_Update_Drive_State();
		_Send_MC_Command_CAN(_Calculate_MC_Ref());
	}

	if (wiper_ARR++ >= WIPER_PERIOD) {
		_Wiper_Tick();
		wiper_ARR = 0;

#ifdef DEBUG_LEDS
		HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_RESET);
#endif
	}
}
