/*
 * user.c
 *
 *	Made for Shell Eco Marathon by SZEnergy team
 *
 *  Created on: Mar 5, 2026
 *      Author: varma02@Github
 */

#include "user.h"
#include "automatic_strategy.h"
#include "switching_automatic_strategy.h"
#ifdef UART_DEBUG
#include <stdio.h>
#endif

struct {
	VCU_STATE_A A;
	VCU_STATE_B B;
} vcu_state;

struct {
	STW_STATE_BUTTONS A;
	uint8_t ROT1;
	uint8_t ROT2;
	uint8_t ROT3;
} stw_state;

struct {
	float wheel_rpm; // rotations per minute
	float speed;     // kilometers per hour
	float distance;  // meters
} vehicle_state;

struct {
	float time;       // seconds
	float prev_time;  // seconds
	float total_diff; // seconds
	uint8_t number;
} lap;

struct {
	DW_automatic_strategy_T internal;
	float torque_gain;
	float torque_base;
	float torque_ref;
} auto_strat_state;

struct {
	DRIVE_MODE_ENUM mode;
	uint8_t setting;
	float torque_ref;
} drive_state;

uint16_t throttle_adc_buffer[10];
float throttle_pedal;

struct {
  uint8_t ARR;
  uint8_t running;
  uint8_t step;
} wiper_state;

struct {
	float current;
	float prev;
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
 * Reads, filters and debounces the current value of the potentiometer
 * Can be configured from `user.h` with the POT_* variables.
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

  throttle_pedal = (average - POT_ZERO) / (POT_MAX - POT_ZERO);
  if (throttle_pedal < 0) {
    throttle_pedal = 0;
  } else if (throttle_pedal > 1) {
    throttle_pedal = 1;
  }

#ifdef UART_DEBUG
	Debug_Msg("POT_ADC_FLTR", average);
#endif
}

void _Update_Vehicle_State() {
	vehicle_state.speed = vehicle_state.wheel_rpm * SPEED_MULT_FACTOR;
	vehicle_state.distance += vehicle_state.speed / 72;
	if (lap.number != 0) {
		lap.time += (float)0.05;
	} else {
		lap.time = OPTIMAL_LAP;
	}
	if (stw_state.A.LAP == SET && (lap.time > 5 || lap.number == 0)) {
		vehicle_state.distance = 0;
		lap.number++;
		lap.prev_time = lap.time;
		lap.total_diff +=  lap.time - OPTIMAL_LAP;
		lap.time = 0;
	}
//	if (stw_state.A.FUNCTION2 == SET) {
//		_Reset_Variables();
//	}
}

/**
 * Calculates the torque reference value to send to the motor controller.
 * Uses the lookup tables from `user.h` in certain switch positions.
 * @return - unsigned integer value
 */
void _Calculate_MC_Ref() {
	drive_state.torque_ref = 0;
	drive_state.mode = DM_NEUTRAL;
	drive_state.setting = 0;

	if (vcu_state.A.MC_OW == SET || vcu_state.A.AUTONOMOUS == SET || vcu_state.A.BRAKE != RESET || (stw_state.A.REVERSE != SET && stw_state.A.DRIVE != SET)) {
		return;
	}

	if (stw_state.A.ACC == SET) {
		switch (stw_state.ROT3) {
			case ROT_1:
				drive_state.mode = DM_MANUAL;
				switch (stw_state.ROT1) {
					case ROT_1:
						drive_state.torque_ref = 0.8;
						drive_state.setting = 1;
						break;
					case ROT_2:
						drive_state.torque_ref = 0.9;
						drive_state.setting = 2;
						break;
					case ROT_3:
						drive_state.torque_ref = 1;
						drive_state.setting = 3;
						break;
				}
				break;
			case ROT_2:
				drive_state.mode = DM_MANUAL_STRATEGY;
				switch (stw_state.ROT1) {
					case ROT_1:
						drive_state.setting = 1;
						if (vehicle_state.wheel_rpm < 116) {
							drive_state.torque_ref = 1;
						} else {
							drive_state.torque_ref = LUT_Z22[(uint16_t)vehicle_state.speed];
						}
						break;
					case ROT_2:
						drive_state.setting = 2;
						if (vehicle_state.wheel_rpm < 221) {
							drive_state.torque_ref = 1;
						} else {
							drive_state.torque_ref = LUT_Z24[(uint16_t)vehicle_state.speed];
						}
						break;
					case ROT_3:
						drive_state.setting = 3;
						if (vehicle_state.wheel_rpm < 224) {
							drive_state.torque_ref = 1;
						} else {
							drive_state.torque_ref = (float)0.332217618;
						}
						break;
				}
				break;
			case ROT_3:
				drive_state.mode = DM_AUTOMATIC_STRATEGY;
				switch (stw_state.ROT1) {
					case ROT_1:
						drive_state.setting = 1;
						automatic_strategy(
								vehicle_state.speed / 3.6F,
								vehicle_state.distance,
								lap.time,
								vehicle_state.wheel_rpm,
								lap.total_diff,
								&auto_strat_state.torque_ref,
								&auto_strat_state.torque_gain,
								&auto_strat_state.torque_base,
								&auto_strat_state.internal
						);
						drive_state.torque_ref = auto_strat_state.torque_ref;
					case ROT_2:
						drive_state.setting = 2;
						switching_automatic_strategy(
								vehicle_state.speed / 3.6F,
								vehicle_state.distance,
								lap.time,
								vehicle_state.wheel_rpm,
								lap.total_diff,
								&auto_strat_state.torque_ref,
								&auto_strat_state.torque_gain,
								&auto_strat_state.torque_base
						);
						drive_state.torque_ref = auto_strat_state.torque_ref;
				}
				break;
			case ROT_4:
				drive_state.mode = DM_SPEED_HOLD;
				//uint8_t target_speed = SPEED_ROT_MAP[stw_state.ROT1];

				break;
		}
	}

	if (throttle_pedal > drive_state.torque_ref) {
		drive_state.mode = DM_MANUAL;
		drive_state.setting = 0;
		drive_state.torque_ref = throttle_pedal;
	}

	if (drive_state.torque_ref != 0) {
		drive_state.torque_ref = _Rate_Limit(drive_state.torque_ref);
	} else {
		rate_limiter.current = 0;
		rate_limiter.prev = 0;
	}


#ifdef UART_DEBUG
	Debug_Msg("MC_REF", drive_state.torque_ref);
#endif
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
		stw_state.ROT1 = data[1];
		stw_state.ROT2 = data[2];
		stw_state.ROT3 = data[3];
	} else {
		User_Error_Handler(UERR_CAN_RECEIVE, 0);
	}

	if (HAL_CAN_GetRxMessage(_usr_can, CAN_RX_FIFO1, &header, data) == HAL_OK) {
		vehicle_state.wheel_rpm = (((uint16_t)data[0] << 8) | data[1]) / 100;
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

	int32_t throttle_buffer;
	if (drive_state.torque_ref != 0) {
		throttle_buffer = drive_state.torque_ref * 100000;
	} else {
		throttle_buffer = throttle_pedal * 100000;
	}
	data[2] = throttle_buffer >> 24;
	data[3] = throttle_buffer >> 16;
	data[4] = throttle_buffer >> 8;
	data[5] = throttle_buffer;

	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}

	if (HAL_CAN_AddTxMessage(_usr_can, &header, data, &mailbox) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
		return;
	}
#ifdef DEBUG_LEDS
	HAL_GPIO_TogglePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin);
#endif
}

void _Send_Auto_Strat_Debug_CAN() {
	CAN_TxHeaderTypeDef header;
	uint8_t data[6];
	uint32_t mailbox;

	header.IDE = CAN_ID_STD;
	header.StdId = 0x150;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 7;

	data[0] = lap.number;
	uint32_t lap_time_buffer = lap.time * 100;
	data[1] = lap_time_buffer >> 8;
	data[2] = lap_time_buffer;
	uint32_t distance_buffer = vehicle_state.distance * 10;
	data[3] = distance_buffer >> 8;
	data[4] = distance_buffer;
	data[5] = drive_state.mode;
	data[6] = drive_state.setting;

	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}

	if (HAL_CAN_AddTxMessage(_usr_can, &header, data, &mailbox) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
		return;
	}

	CAN_TxHeaderTypeDef header2;
	uint8_t data2[6];
	uint32_t mailbox2;

	header2.IDE = CAN_ID_STD;
	header2.StdId = 0x151;
	header2.RTR = CAN_RTR_DATA;
	header2.DLC = 4;

	uint32_t torque_gain_buffer = auto_strat_state.torque_gain * 100;
	data2[0] = torque_gain_buffer >> 8;
	data2[1] = torque_gain_buffer;
	uint32_t torque_base_buffer = auto_strat_state.torque_base * 100;
	data2[2] = torque_base_buffer >> 8;
	data2[3] = torque_base_buffer;

	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}

	if (HAL_CAN_AddTxMessage(_usr_can, &header2, data2, &mailbox2) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
		return;
	}
}

void _Send_BMS_Query_CAN() {
	CAN_TxHeaderTypeDef header;
	uint8_t data[8] = {0,0,0,0,0,0,0,0};
	uint32_t mailbox;

	header.IDE = CAN_ID_EXT;
	header.ExtId = 0x0400FF80;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 8;

	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}

	if (HAL_CAN_AddTxMessage(_usr_can, &header, data, &mailbox) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
		return;
	}
}

void _Send_SMAG_Init_CAN() {
	CAN_TxHeaderTypeDef header1;
	uint8_t data1[8] = {0x2B, 0, 0x62, 0, 0x32, 0, 0, 0};
	uint32_t mailbox1;

	header1.IDE = CAN_ID_STD;
	header1.StdId = 0x605;
	header1.RTR = CAN_RTR_DATA;
	header1.DLC = 8;

	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}

	if (HAL_CAN_AddTxMessage(_usr_can, &header1, data1, &mailbox1) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
		return;
	}

	HAL_Delay(500);

	CAN_TxHeaderTypeDef header2;
	uint8_t data2[8] = {0x1, 0x5, 0, 0, 0, 0, 0, 0};
	uint32_t mailbox2;

	header2.IDE = CAN_ID_STD;
	header2.StdId = 0x0;
	header2.RTR = CAN_RTR_DATA;
	header2.DLC = 8;

	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}

	if (HAL_CAN_AddTxMessage(_usr_can, &header2, data2, &mailbox2) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
		return;
	}
}

/**
 * Sends the torque reference to the motor controller over CAN.
 */
void _Send_MC_Command_CAN() {
	CAN_TxHeaderTypeDef header;
	uint8_t data[4];
	uint32_t mailbox;

	header.IDE = CAN_ID_EXT;
	header.ExtId = 0xA51;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 4;

	int32_t throttle_buffer = drive_state.torque_ref * 100000;
	if (stw_state.A.REVERSE == SET) {
		throttle_buffer = throttle_buffer * -1;
	}
	data[0] = throttle_buffer >> 24;
	data[1] = throttle_buffer >> 16;
	data[2] = throttle_buffer >> 8;
	data[3] = throttle_buffer;

	while (HAL_CAN_GetTxMailboxesFreeLevel(_usr_can) == 0) {}

	if (HAL_CAN_AddTxMessage(_usr_can, &header, data, &mailbox) != HAL_OK) {
		User_Error_Handler(UERR_CAN_MSG_SEND, 0);
	}
}

/**
 * This is the periodic wiper task.
 * It checks for the wiper switch to be turned on, then executes the left-right motion.
 * This also turns on/off the wiper DC-DC converter for the servo.
 */
void _Wiper_Tick() {
	switch (wiper_state.step) {
	case 0: // left here for compatibility
    wiper_state.step = 1;
    break;
	case 1: // Setup phase, start PWM and converter
	  if (vcu_state.A.WIPER != SET) return;
		HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_SET);
		HAL_TIM_PWM_Start(_usr_wiper_pwm, TIM_CHANNEL_1);
		wiper_state.running = SET;
		wiper_state.step = 2;
		break;
#ifdef WIPER_SWEEP_DEBUG
	case 2:
		for (uint16_t i = 0; i < WIPER_TIM_ARR-20; i+=20){
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
	vcu_state.A.bits = 0;
	vcu_state.B.bits = 0;
	stw_state.A.bits = 0;
	stw_state.ROT1 = 0;
	stw_state.ROT2 = 0;
	stw_state.ROT3 = 0;
	vehicle_state.distance = 0;
	vehicle_state.speed = 0;
	vehicle_state.wheel_rpm = 0;
	drive_state.mode = DM_NEUTRAL;
	drive_state.setting = 0;
	drive_state.torque_ref = 0;
	auto_strat_state.internal.DiscreteTimeIntegrator_DSTATE = 0;
	auto_strat_state.internal.DelayInput1_DSTATE = 0;
	auto_strat_state.internal.DiscreteTimeIntegrator_PrevRese = 0;
	automatic_strategy_Init(&auto_strat_state.internal);
	auto_strat_state.torque_base = 0;
	auto_strat_state.torque_gain = 0;
	auto_strat_state.torque_ref = 0;
	lap.number = 0;
	lap.prev_time = OPTIMAL_LAP;
	lap.total_diff = 0;
	lap.time = 0;
	for (uint8_t i = 0; i < 10; i++) {
		throttle_adc_buffer[i] = 0;
	}
	throttle_pedal = 0;
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

	HAL_Delay(500);
	_Send_SMAG_Init_CAN();

#ifdef DEBUG_LEDS
	HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_SET);
#endif
}

/**
 * Executes one tick.
 */
void User_Loop() {
	_Read_GPIO_Inputs();

	HAL_GPIO_WritePin(Brake_light_GPIO_Port, Brake_light_Pin, vcu_state.A.BRAKE == RESET);

	_Receive_CAN();

	_Pot_Filter();

	_Update_Vehicle_State();

	_Calculate_MC_Ref();

	_Send_VCU_State_CAN();

	if (vcu_state.A.MC_OW == RESET && vcu_state.A.AUTONOMOUS == RESET) {
		_Send_MC_Command_CAN(drive_state.torque_ref);
	}

	_Send_Auto_Strat_Debug_CAN();

	if (wiper_state.ARR++ >= WIPER_PERIOD) {
		_Wiper_Tick();
		wiper_state.ARR = 0;

		_Send_BMS_Query_CAN();
	}

}
