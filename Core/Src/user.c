/*
 * user.c
 *
 *	Made for Shell Eco Marathon by SZEnergy team
 *
 *  Created on: Mar 5, 2026
 *      Author: varma02@Github
 */

#include "user.h"
#include "ltv_lqr_strategy.h"
#include "pesc_sleep.h"

#ifdef UART_DEBUG
#include <stdio.h>
#endif

struct {
	VCU_STATE_A A;
	VCU_STATE_B B;
} switch_table_state;

struct {
	STW_STATE_BUTTONS A;
	uint8_t ROT1;
	uint8_t ROT2;
	uint8_t ROT3;
} steering_wheel_state; // steering wheel

struct {
	float wheel_rpm; // rotations per minute
	float speed;     // kilometers per hour
	float distance;  // meters
	uint8_t lap_number;
	float laptime;       // seconds
	float total_time_diff; // seconds
} vehicle_state;

struct {
	float torque_gain;
	float torque_base;
	float speed_ref;
	float distance_ref;
} simulink_debug;

DW_ltv_lqr_strategy_T ltv_lqr_internal_state;
float speed_hold_I;

uint16_t throttle_adc_buffer[10];
struct {
	DRIVE_MODE_ENUM prev_mode;
	DRIVE_MODE_ENUM mode;
	uint8_t setting;
	float torque_ref_out;
	float throttle_pedal;
} drive_state;

struct {
  uint16_t ARR;
  uint8_t running;
  uint8_t step;
} wiper_state;

struct {
	float current;
	float prev;
} rate_limiter;

uint16_t bms_query_ARR;
uint16_t smag_init_ARR;

CAN_HandleTypeDef *_usr_can;
TIM_HandleTypeDef *_usr_wiper_pwm;
UART_HandleTypeDef *_usr_uart;

void _Reset_Simulink_States() {
	simulink_debug.torque_base = 0;
	simulink_debug.torque_gain = 0;
	simulink_debug.speed_ref = 0;
	simulink_debug.distance_ref = 0;
	ltv_lqr_internal_state.DiscreteTimeIntegrator_DSTATE = 0;
	ltv_lqr_internal_state.DiscreteTimeIntegrator_PrevRese = 0;
	ltv_lqr_strategy_Init(&ltv_lqr_internal_state);
	speed_hold_I = 0;
}

/**
 * Resets all global variables to their default values.
 */
void _Reset_Variables() {
	switch_table_state.A.bits = 0;
	switch_table_state.B.bits = 0;
	steering_wheel_state.A.bits = 0;
	steering_wheel_state.ROT1 = 0;
	steering_wheel_state.ROT2 = 0;
	steering_wheel_state.ROT3 = 0;
	vehicle_state.distance = 0;
	vehicle_state.speed = 0;
	vehicle_state.wheel_rpm = 0;
	vehicle_state.lap_number = 0;
	vehicle_state.total_time_diff = 0;
	vehicle_state.laptime = 0;
	drive_state.mode = DM_NEUTRAL;
	drive_state.setting = 0;
	drive_state.torque_ref_out = 0;
	drive_state.throttle_pedal = 0;
	for (uint8_t i = 0; i < 10; i++) {
		throttle_adc_buffer[i] = 0;
	}
	rate_limiter.current = 0;
	rate_limiter.prev = 0;
	wiper_state.running = RESET;
	wiper_state.step = 0;
	wiper_state.ARR = 0;
	smag_init_ARR = 0;
	_Reset_Simulink_States();
}


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
 * For non-critical errors it just flashes the Yellow LED and logs on UART (if available).
 * If the error is fatal and the program cannot continue, it flashes on the Red LED in an infinite loop.
 * @param err - a value from the USER_ERROR enum
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
		while (1) {}
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
 * @returns - rate limited value
 */
float _Rate_Limit(float value) {
	rate_limiter.prev = rate_limiter.current;
	rate_limiter.current = value;
	if (rate_limiter.prev + RATE_LIMIT_UP < rate_limiter.current) {
		rate_limiter.current = rate_limiter.prev + RATE_LIMIT_UP;
	} else if (rate_limiter.prev - RATE_LIMIT_DOWN > rate_limiter.current) {
		rate_limiter.current = rate_limiter.prev - RATE_LIMIT_DOWN;
	}
    return rate_limiter.current;
}

/**
 * This function reads the DMA variable `throttle_adc_buffer` global,
 * averages and filters the values, then puts it into the `drive_state.throttle_pedal` global.
 * Limits can be configured from `user.h` with the POT_ZERO and POT_MAX variables.
 */
void _Pot_Filter() {
  uint32_t average = 0;
	for (uint8_t i = 0; i < 10; i++){
		average += throttle_adc_buffer[i];
	}
	average = average / 10;

#ifdef UART_DEBUG
	Debug_Msg("POT_ADC_AVG", average);
#endif

  drive_state.throttle_pedal = (average - POT_ZERO) / (POT_MAX - POT_ZERO);
  if (drive_state.throttle_pedal < 0) {
    drive_state.throttle_pedal = 0;
  } else if (drive_state.throttle_pedal > 1) {
    drive_state.throttle_pedal = 1;
  }

#ifdef UART_DEBUG
	Debug_Msg("POT_ADC_FLTR", average);
#endif
}

/**
 * Updates `lap` and `vehicle_state` globals.
 * Should be called after getting steering wheel and encoder inputs.
 */
void _Update_Vehicle_State() {
	vehicle_state.speed = vehicle_state.wheel_rpm * SPEED_MULT_FACTOR;
	vehicle_state.distance += vehicle_state.speed / 72;
	if (vehicle_state.lap_number == 0) {
		vehicle_state.laptime = OPTIMAL_LAP;
	} else {
		vehicle_state.laptime += (float)0.05;
	}
	if (steering_wheel_state.A.LAP == SET && \
	(vehicle_state.lap_number == 0 || (vehicle_state.laptime > LAP_BTN_MIN_TIME && vehicle_state.distance > LAP_BTN_MIN_DISTANCE))) {
		vehicle_state.lap_number++;
		vehicle_state.distance = 0;
		vehicle_state.total_time_diff +=  vehicle_state.laptime - OPTIMAL_LAP;
		vehicle_state.laptime = 0;
	}

	// Steering wheel reset button restarts the switch table
	if (steering_wheel_state.A.FUNCTION1 == SET) {
		NVIC_SystemReset();
	}
}

/**
 * Determines the current drive mode and setting, then calculates the correct throttle reference.
 * Uses lookup tables and values from `user.h` in certain modes.
 */
void _Calculate_MC_Ref() {
	drive_state.prev_mode = drive_state.mode;
	drive_state.torque_ref_out = 0;
	drive_state.mode = DM_NEUTRAL;
	drive_state.setting = 0;
	switch_table_state.A.PESC_SLEEP = RESET;

	if (steering_wheel_state.A.REVERSE != SET && steering_wheel_state.A.DRIVE != SET) {
		switch_table_state.A.PESC_SLEEP = SET;
		return;
	}

	float ltv_lqr_torque_ref_out = 0;
	float ltv_lqr_torque_base = 0;
	float ltv_lqr_torque_gain = 0;
	float ltv_lqr_speed_ref = 0;
	float ltv_lqr_distance_ref = 0;
	ltv_lqr_strategy(
		vehicle_state.speed / 3.6F,
		vehicle_state.distance,
		vehicle_state.laptime,
		vehicle_state.wheel_rpm,
		vehicle_state.total_time_diff,
		&ltv_lqr_torque_ref_out,
		&ltv_lqr_torque_gain,
		&ltv_lqr_torque_base,
		&ltv_lqr_speed_ref,
		&ltv_lqr_distance_ref,
		&ltv_lqr_internal_state
	);


	switch (steering_wheel_state.ROT3) {
		case ROT_1: // Manual
			drive_state.mode = DM_MANUAL;
			switch (steering_wheel_state.ROT1) {
				case ROT_1:
					drive_state.torque_ref_out = 0.8;
					drive_state.setting = 1;
					break;
				case ROT_2:
					drive_state.torque_ref_out = 0.9;
					drive_state.setting = 2;
					break;
				case ROT_3:
					drive_state.torque_ref_out = 1;
					drive_state.setting = 3;
					break;
			}
			break;
		case ROT_2: // Manual strategy
			drive_state.mode = DM_MANUAL_STRATEGY;
			switch (steering_wheel_state.ROT1) {
				case ROT_1:  // Aumovio test track Z22
					drive_state.setting = 3;
					if (vehicle_state.wheel_rpm < 224) {
						drive_state.torque_ref_out = 1;
					} else {
						drive_state.torque_ref_out = (float)0.332217618;
					}
					break;
				case ROT_2: // Silesia Ring Z22
					drive_state.setting = 1;
					if (vehicle_state.wheel_rpm < 159.388F) {
						drive_state.torque_ref_out = 1;
					} else {
						drive_state.torque_ref_out = (float)0.381551;
					}
					break;
			}
			break;
		case ROT_3: // Automatic strategy
			drive_state.mode = DM_AUTOMATIC_STRATEGY;
			switch (steering_wheel_state.ROT1) {
				case ROT_1:
					drive_state.setting = 1;
					simulink_debug.torque_base = ltv_lqr_torque_base;
					simulink_debug.torque_gain = ltv_lqr_torque_gain;
					simulink_debug.speed_ref = ltv_lqr_speed_ref;
					simulink_debug.distance_ref = ltv_lqr_distance_ref;
					drive_state.torque_ref_out = ltv_lqr_torque_ref_out;
					break;
				case ROT_2:
//					drive_state.setting = 2;
//					simulink_debug.torque_base = switching_lqr_torque_base;
//					simulink_debug.torque_gain = switching_lqr_torque_gain;
//					drive_state.torque_ref_out = switching_lqr_torque_ref_out;
					break;
			}
			break;
		case ROT_4: // Speed hold
			drive_state.mode = DM_SPEED_HOLD;
			drive_state.setting = ROT_TO_INT[steering_wheel_state.ROT1];
			uint8_t target_speed = drive_state.setting * 5;
			simulink_debug.speed_ref = vehicle_state.speed;
			float pid_temp = (target_speed - vehicle_state.speed) * 0.25F;
			simulink_debug.torque_base = pid_temp;
			speed_hold_I += pid_temp * 0.1F;
			if (speed_hold_I > 1) speed_hold_I = 1;
			else if (speed_hold_I < -1) speed_hold_I = -1;
			drive_state.torque_ref_out = pid_temp + speed_hold_I;
			simulink_debug.torque_gain = speed_hold_I;
			break;
		case ROT_5: // Race mode
			switch (steering_wheel_state.ROT1) {
				case ROT_1: // Aumovio test track
					if (vehicle_state.lap_number == 2) {
						vehicle_state.total_time_diff = 0;
					}
					if (vehicle_state.lap_number <= 1) {
						drive_state.mode = DM_MANUAL_STRATEGY;
						drive_state.setting = 3;
						if (vehicle_state.wheel_rpm < 224) {
							drive_state.torque_ref_out = 1;
						} else {
							drive_state.torque_ref_out = (float)0.332217618;
						}
					} else {
						drive_state.mode = DM_AUTOMATIC_STRATEGY;
						drive_state.setting = 1;
						simulink_debug.torque_base = ltv_lqr_torque_base;
						simulink_debug.torque_gain = ltv_lqr_torque_gain;
						simulink_debug.speed_ref = ltv_lqr_speed_ref;
						simulink_debug.distance_ref = ltv_lqr_distance_ref;
						drive_state.torque_ref_out = ltv_lqr_torque_ref_out;
						switch_table_state.A.PESC_SLEEP = pesc_sleep(vehicle_state.distance);
					}
					break;
				case ROT_2: // Silesia Ring
					if (vehicle_state.lap_number == 2) {
						vehicle_state.total_time_diff = 0;
					}
					if (vehicle_state.lap_number <= 1) {
						drive_state.mode = DM_MANUAL_STRATEGY;
						drive_state.setting = 1;
						if (vehicle_state.wheel_rpm < 159.388F) {
							drive_state.torque_ref_out = 1;
						} else {
							drive_state.torque_ref_out = (float)0.381551;
						}
					} else {
						drive_state.mode = DM_AUTOMATIC_STRATEGY;
						drive_state.setting = 1;
						simulink_debug.torque_base = ltv_lqr_torque_base;
						simulink_debug.torque_gain = ltv_lqr_torque_gain;
						simulink_debug.speed_ref = ltv_lqr_speed_ref;
						simulink_debug.distance_ref = ltv_lqr_distance_ref;
						drive_state.torque_ref_out = ltv_lqr_torque_ref_out;
						switch_table_state.A.PESC_SLEEP = pesc_sleep(vehicle_state.distance);
					}
					break;
			}
			break;
	}

	if (switch_table_state.A.BRAKE != RESET) {
		drive_state.torque_ref_out = 0;
		return;
	}

	if (steering_wheel_state.A.ACC != SET) {
		drive_state.torque_ref_out = 0;
	}

	if (drive_state.torque_ref_out < 0) {
		drive_state.torque_ref_out = 0;
	} else if (drive_state.torque_ref_out > 1) {
		drive_state.torque_ref_out = 1;
	}

	if (drive_state.throttle_pedal > drive_state.torque_ref_out) {
		drive_state.mode = DM_MANUAL;
		drive_state.setting = 0;
		drive_state.torque_ref_out = drive_state.throttle_pedal;
		switch_table_state.A.PESC_SLEEP = RESET;
	}

	if (switch_table_state.A.AUTONOMOUS == SET) {
		switch_table_state.A.PESC_SLEEP = RESET;
	}

	if (drive_state.mode != drive_state.prev_mode) {
		_Reset_Simulink_States();
	}

	if (drive_state.torque_ref_out != 0) {
		drive_state.torque_ref_out = _Rate_Limit(drive_state.torque_ref_out);
	} else {
		rate_limiter.current = 0;
		rate_limiter.prev = 0;
	}


#ifdef UART_DEBUG
	Debug_Msg("MC_REF", drive_state.torque_ref_out);
#endif
}

void _Read_GPIO_Inputs() {
	switch_table_state.A.AUTONOMOUS     = HAL_GPIO_ReadPin(Autonomous_switch_GPIO_Port, Autonomous_switch_Pin);
	switch_table_state.A.LIGHTS_HAZARD  = HAL_GPIO_ReadPin(Hazard_switch_GPIO_Port, Hazard_switch_Pin);
	switch_table_state.A.LIGHTS_ENABLE  = HAL_GPIO_ReadPin(Lights_enable_switch_GPIO_Port, Lights_enable_switch_Pin);
	switch_table_state.A.MC_OW          = HAL_GPIO_ReadPin(Motorcontrol_override_switch_GPIO_Port, Motorcontrol_override_switch_Pin);
	switch_table_state.A.WIPER          = HAL_GPIO_ReadPin(Wiper_switch_GPIO_Port, Wiper_switch_Pin);
	switch_table_state.A.LIGHTS_DRL     = HAL_GPIO_ReadPin(Headlight_switch_GPIO_Port, Headlight_switch_Pin);
	switch_table_state.A.BRAKE          = HAL_GPIO_ReadPin(Brake_pedal_input_GPIO_Port, Brake_pedal_input_Pin);
}


/**
 * Initializes the CAN2 peripheral and starts it.
 * Applies filters that send message 0x190 (steering wheel) to FIFO0,
 * and 0x123 (encoder) to FIFO1 for easy processing.
 */
void _Init_CAN() {
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
		User_Error_Handler(UERR_CAN_FILTER_CONFIG, SET);
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
		User_Error_Handler(UERR_CAN_FILTER_CONFIG, SET);
	}

	if (HAL_CAN_Start(_usr_can) != HAL_OK)
	{
		User_Error_Handler(UERR_CAN_START, SET);
	}
}

/**
 * Gets the received CAN messages and stores their signals in global variables.
 * - the button/switch inputs from the steering wheel
 * - the current wheel RPM value from the encoder
 */
void _Receive_CAN() {
	CAN_RxHeaderTypeDef header;
	uint8_t data[8];

	if (HAL_CAN_GetRxMessage(_usr_can, CAN_RX_FIFO0, &header, data) == HAL_OK) {
		steering_wheel_state.A.bits = data[0];
		steering_wheel_state.ROT1 = data[1];
		steering_wheel_state.ROT2 = data[2];
		steering_wheel_state.ROT3 = data[3];
	} else {
		User_Error_Handler(UERR_CAN_RECEIVE_STW, RESET);
	}

	if (HAL_CAN_GetRxMessage(_usr_can, CAN_RX_FIFO1, &header, data) == HAL_OK) {
		vehicle_state.wheel_rpm = (((uint16_t)data[0] << 8) | data[1]) / 100;
	} else {
		User_Error_Handler(UERR_CAN_RECEIVE_ENCODER, RESET);
	}
}

/**
 * Helper function for sending CAN messages.
 * `is_ext` is a boolean value, it tells the function if the ID provided is extended (SET) or standard (RESET)
 */
void _Generic_CAN_Send(uint8_t is_ext, uint32_t id, uint8_t bytes, uint8_t *data) {
	CAN_TxHeaderTypeDef header;
	if (is_ext == SET) {
		header.IDE = CAN_ID_EXT;
		header.ExtId = id;
	} else {
		header.IDE = CAN_ID_STD;
		header.StdId = id;
	}
	header.RTR = CAN_RTR_DATA;
	header.DLC = bytes;

	// wait for available mailbox, then send
	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}
	uint32_t mailbox;
	if (HAL_CAN_AddTxMessage(_usr_can, &header, data, &mailbox) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, RESET);
	}
}

void _Send_VCU_State_CAN() {
	uint8_t data[6];

	data[0] = switch_table_state.A.bits;
	data[1] = switch_table_state.B.bits;

	int32_t throttle_buffer;
	if (drive_state.torque_ref_out != 0) {
		throttle_buffer = drive_state.torque_ref_out * 100000;
	} else {
		throttle_buffer = drive_state.throttle_pedal * 100000;
	}
	data[2] = throttle_buffer >> 24;
	data[3] = throttle_buffer >> 16;
	data[4] = throttle_buffer >> 8;
	data[5] = throttle_buffer;

	_Generic_CAN_Send(RESET, 0x129, 6, data);

#ifdef DEBUG_LEDS
	HAL_GPIO_TogglePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin);
#endif
}

void _Send_VCU_Calculated_State_CAN() {
	uint8_t data_0x150[8];

	data_0x150[0] = vehicle_state.lap_number;

	uint32_t lap_time_buffer = vehicle_state.laptime * 100.0F;
	data_0x150[1] = lap_time_buffer >> 8;
	data_0x150[2] = lap_time_buffer;

	uint32_t distance_buffer = vehicle_state.distance * 20.0F;
	data_0x150[3] = distance_buffer >> 8;
	data_0x150[4] = distance_buffer;

	data_0x150[5] = drive_state.mode << 4 | drive_state.setting;

	int16_t total_time_diff_buffer = vehicle_state.total_time_diff * 100.0F;
	data_0x150[6] = total_time_diff_buffer >> 8;
	data_0x150[7] = total_time_diff_buffer;

	_Generic_CAN_Send(RESET, 0x150, 8, data_0x150);
}

void _Send_Simulink_Debug_CAN() {
	uint8_t data_0x151[8];

	int32_t torque_gain_buffer = simulink_debug.torque_base * 100.0F;
	data_0x151[0] = torque_gain_buffer >> 8;
	data_0x151[1] = torque_gain_buffer;

	int32_t torque_base_buffer = simulink_debug.torque_gain * 100.0F;
	data_0x151[2] = torque_base_buffer >> 8;
	data_0x151[3] = torque_base_buffer;

	uint32_t speed_ref_buffer = simulink_debug.speed_ref * 1000.0F;
	data_0x151[4] = speed_ref_buffer >> 8;
	data_0x151[5] = speed_ref_buffer;

	uint32_t distance_ref_buffer = simulink_debug.distance_ref * 200.0F;
	data_0x151[6] = distance_ref_buffer >> 8;
	data_0x151[7] = distance_ref_buffer;

	_Generic_CAN_Send(RESET, 0x151, 8, data_0x151);
}

void _Send_BMS_Query_CAN() {
	uint8_t data[8] = {0,0,0,0,0,0,0,0};
	_Generic_CAN_Send(SET, 0x0400FF80, 8, data);
}

void _Send_MC_Command_CAN() {
	uint8_t data[4];
	int32_t throttle_buffer = drive_state.torque_ref_out * 100000;
	if (steering_wheel_state.A.REVERSE == SET) {
		throttle_buffer = throttle_buffer * -1;
	}
	data[0] = throttle_buffer >> 24;
	data[1] = throttle_buffer >> 16;
	data[2] = throttle_buffer >> 8;
	data[3] = throttle_buffer;

	_Generic_CAN_Send(SET, 0xA51, 4, data);
}

/**
 * This is the periodic wiper task.
 * It checks for the wiper switch to be turned on, then executes the left-right motion.
 * This also turns on/off the wiper DC-DC converter for the servo.
 */
void _Wiper_Tick(uint8_t wiper_switch) {
	switch (wiper_state.step) {
	case 0: // left here for compatibility
		wiper_state.step = 1;
		break;
	case 1: // Setup phase, start PWM and converter
		if (wiper_switch != SET) return;
		HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_SET);
		HAL_TIM_PWM_Start(_usr_wiper_pwm, TIM_CHANNEL_1);
		wiper_state.running = SET;
		wiper_state.step = 2;
		break;
#ifdef WIPER_SWEEP_DEBUG
	case 2:
		for (uint16_t i = 0; i < 40000-20; i+=20){
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
		if (wiper_switch == RESET) {
			wiper_state.step = 4;
		} else {
			_usr_wiper_pwm->Instance->CCR1 = WIPER_RIGHT;
			wiper_state.step = 3;
		}
		break;
	case 3: // Wipe Left
		if (wiper_switch == RESET) {
			wiper_state.step = 4;
		} else {
			_usr_wiper_pwm->Instance->CCR1 = WIPER_LEFT;
			wiper_state.step = 2;
		}
		break;
	case 4: // Go to the center
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
	}
#ifdef UART_DEBUG
	Debug_Msg("WIPER_RUN", wiper_state.running);
	Debug_Msg("WIPER_STEP", wiper_state.step);
#endif
}


void _Reset_Outputs() {
	HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_Red_GPIO_Port, LED2_Red_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Brake_light_GPIO_Port, Brake_light_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Debug_Out_GPIO_Port, Debug_Out_Pin, GPIO_PIN_RESET);
}

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

void User_Loop() {
	_Read_GPIO_Inputs();

	// break light output is inverted for some reason
	HAL_GPIO_WritePin(Brake_light_GPIO_Port, Brake_light_Pin, switch_table_state.A.BRAKE == RESET);

	_Receive_CAN();

	_Pot_Filter();

	_Update_Vehicle_State();

	_Calculate_MC_Ref();

	_Send_VCU_State_CAN();

	if (switch_table_state.A.MC_OW == RESET && switch_table_state.A.AUTONOMOUS == RESET) {
		_Send_MC_Command_CAN(drive_state.torque_ref_out);
	}

	_Send_VCU_Calculated_State_CAN();
	_Send_Simulink_Debug_CAN();

	if (wiper_state.ARR++ >= WIPER_PERIOD) {
		_Wiper_Tick(switch_table_state.A.WIPER);
		wiper_state.ARR = 0;
	}

	if (bms_query_ARR++ >= BMS_QUERY_PERIOD) {
		_Send_BMS_Query_CAN();
		bms_query_ARR = 0;
	}

	if (smag_init_ARR <= 150) {
		if (smag_init_ARR == 100) {
			uint8_t data_0x605[8] = {0x2B, 0, 0x62, 0, 0x32, 0, 0, 0};
			_Generic_CAN_Send(RESET, 0x605, 8, data_0x605);
		} else if (smag_init_ARR == 150) {
			uint8_t data_0x0[8] = {0x1, 0x5, 0, 0, 0, 0, 0, 0};
			_Generic_CAN_Send(RESET, 0x0, 8, data_0x0);
		}
		smag_init_ARR++;
	}
}
