/*
 * user.c
 *
 *	Made for Shell Eco Marathon by SZEnergy team
 *
 *  Created on: Mar 5, 2026
 *      Author: varma02@Github
 */

#include "user.h"

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

volatile struct USER_FLAGS user_flags;

double wheel_rpm;

struct {
	DRIVE_STATE prev;
	DRIVE_STATE current;
} drive_state;

struct {
	volatile uint32_t adc;
	uint16_t current;
	uint16_t prev;
	uint16_t ema;
} throttle_value;

struct WIPER_STATE _wiper_state;

struct {
	uint16_t current;
	uint16_t prev;
} _rate_limiter;

struct {
	uint8_t can_msg;
	uint16_t wiper_tick;
} _timer_counters;

ADC_HandleTypeDef *_usr_throttle_adc;
CAN_HandleTypeDef *_usr_can;
TIM_HandleTypeDef *_usr_wiper_pwm;
UART_HandleTypeDef *_usr_uart;


void User_Error_Handler(USER_ERROR err, uint8_t fatal) {
	if (fatal == SET) {
		HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_Red_GPIO_Port, LED2_Red_Pin, GPIO_PIN_SET);
		__disable_irq();
		while (1) {
			HAL_GPIO_TogglePin(LED2_Red_GPIO_Port, LED2_Red_Pin);
			HAL_Delay(250);
		}
	} else {
		HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_SET);
	}
}

uint16_t _Rate_Limit(uint16_t value) {
	_rate_limiter.prev = _rate_limiter.current;
	_rate_limiter.current = value;
    if(2000 + _rate_limiter.current - _rate_limiter.prev > 2000 + RATE_LIMIT_UP)
        _rate_limiter.current = _rate_limiter.prev + RATE_LIMIT_UP;
    else if(2000 + _rate_limiter.current - _rate_limiter.prev < 2000 - RATE_LIMIT_DOWN)
        _rate_limiter.current = _rate_limiter.prev - RATE_LIMIT_DOWN;
    return _rate_limiter.current;
}


/**
 * Raw ADC reader function, uses polling, deprecated in favor of interrupts
 */
//uint16_t ADC_Read() {
//	uint16_t value;
//	HAL_ADC_Start(_usr_throttle_adc);
//	if (HAL_ADC_PollForConversion(_usr_throttle_adc, 5) == HAL_OK) {
//		value = HAL_ADC_GetValue(_usr_throttle_adc);
//	}
//	HAL_ADC_Stop(_usr_throttle_adc);
//	return value;
//}


/**
 * Reads, filters and debounces the current value of the potentiometer
 * @uses_global throttle_value, POT_ZERO, POT_EMA, POT_STEP, POT_VALUES
 */
void _Pot_Filter() {
	throttle_value.prev = throttle_value.current;
	throttle_value.current = throttle_value.adc;

//	debug message 1
//	char msg[32];
//	uint32_t len = sprintf(msg, "RAW: %d\r\n", throttle_value.adc);
//	HAL_UART_Transmit(_usr_uart, (uint8_t*)msg, len, 100);

	if (throttle_value.current > POT_ZERO) {
        throttle_value.ema = (POT_EMA * throttle_value.current) + ((1 - POT_EMA) * throttle_value.ema);
        throttle_value.current = throttle_value.ema;
	}

	if ((throttle_value.current - throttle_value.prev) <= POT_STEP)
		throttle_value.current = throttle_value.prev;

	if (throttle_value.current <= POT_ZERO + POT_STEP) {
		throttle_value.current = POT_VALUES[0];
	} else if (throttle_value.current <= POT_ZERO + POT_STEP * 19) {
		throttle_value.current = POT_VALUES[(throttle_value.current - POT_ZERO) / POT_STEP];
	} else {
		throttle_value.current = POT_VALUES[19];
	}

//	debug message 2
//	char msg2[32];
//	uint32_t len2 = sprintf(msg2, "FILTER: %d\r\n", throttle_value.current);
//	HAL_UART_Transmit(_usr_uart, (uint8_t*)msg2, len2, 100);
}

/**
 * Updates the drive state machine
 * @uses_global drive_state, stw_state, vcu_state
 */
void _Update_Drive_State() {
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
}

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
		_rate_limiter.current = 0;
		_rate_limiter.prev = 0;
		reference = 0;
	} else {
		reference = 0;
	}

	return _Rate_Limit(reference);
}

void _Read_GPIO_Inputs() {
	vcu_state.A.AUTONOMOUS     = HAL_GPIO_ReadPin(Autonomous_switch_GPIO_Port, Autonomous_switch_Pin);
	vcu_state.A.HAZARD         = HAL_GPIO_ReadPin(Hazard_switch_GPIO_Port, Hazard_switch_Pin);
	vcu_state.A.LIGHTS_ENABLE  = HAL_GPIO_ReadPin(Lights_enable_switch_GPIO_Port, Lights_enable_switch_Pin);
	vcu_state.A.MC_OW          = HAL_GPIO_ReadPin(Motorcontrol_override_switch_GPIO_Port, Motorcontrol_override_switch_Pin);
	vcu_state.A.WIPER          = HAL_GPIO_ReadPin(Wiper_switch_GPIO_Port, Wiper_switch_Pin);
	vcu_state.A.HEADLIGHT      = HAL_GPIO_ReadPin(Headlight_switch_GPIO_Port, Headlight_switch_Pin);
	vcu_state.A.BRAKE          = HAL_GPIO_ReadPin(Brake_pedal_input_GPIO_Port, Brake_pedal_input_Pin);
}

void _Receive_CAN() {
	CAN_RxHeaderTypeDef header;
	uint8_t data[8];

	if (HAL_CAN_GetRxMessage(_usr_can, CAN_RX_FIFO0, &header, data) != HAL_OK) {
//		User_Error_Handler(UERR_CAN_RECEIVE, 0);
		return;
	}

	switch (header.StdId) {
//		case 0x185:
//			user_flags.can_synced = SET;
//			break;
		case 0x190:
			stw_state.A.bits = data[0];
			stw_state.B.bits = data[1];
			stw_state.C.bits = data[2];
			stw_state.D.bits = data[3];
			break;
		case 0x123:
			wheel_rpm = ((uint16_t)data[0] << 8) | data[1];
			break;
		default: break;
	}
}

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
}

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


void _Wiper_Tick() {
	switch (_wiper_state.step) {
	case 0: // Standby state, waiting for wiper switch signal
		if (vcu_state.A.WIPER == SET)
			_wiper_state.step = 1;
		break;
	case 1: // Setup phase, start PWM and converter
		_usr_wiper_pwm->Instance->CCR1 = WIPER_LEFT;
		HAL_TIM_PWM_Start(_usr_wiper_pwm, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_SET);
		_wiper_state.running = SET;
		_wiper_state.step = 2;
		break;
	case 2: // Wipe Right
		_usr_wiper_pwm->Instance->CCR1 = WIPER_RIGHT; // Right
		if (vcu_state.A.WIPER == RESET) {
			_wiper_state.step = 4;
		} else {
			_wiper_state.step = 3;
		}
		break;
	case 3: // Wipe Left
		_usr_wiper_pwm->Instance->CCR1 = WIPER_LEFT; // Left
		if (vcu_state.A.WIPER == RESET) {
			_wiper_state.step = 4;
		} else {
			_wiper_state.step = 2;
		}
		break;
	case 4: // Go to the center
		_usr_wiper_pwm->Instance->CCR1 = WIPER_CENTER;
		_wiper_state.step = 5;
		break;
	case 5: // Turn off and go to standby
		HAL_TIM_PWM_Stop(_usr_wiper_pwm, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_RESET);
		_wiper_state.running = RESET;
		_wiper_state.step = 0;
		break;
	default:
		break;
	}
}


void _Init_CAN() {
	CAN_FilterTypeDef filter_1;
	filter_1.FilterBank = 14;
	filter_1.SlaveStartFilterBank = 14;
	filter_1.FilterMode = CAN_FILTERMODE_IDLIST;
	filter_1.FilterScale = CAN_FILTERSCALE_16BIT;
	filter_1.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter_1.FilterActivation = CAN_FILTER_ENABLE;
	filter_1.FilterIdHigh = 0x190 << 5;
	filter_1.FilterIdLow = 0x123 << 5;
	filter_1.FilterMaskIdHigh = 0x190 << 5;
	filter_1.FilterMaskIdLow = 0x123 << 5;
	if (HAL_CAN_ConfigFilter(_usr_can, &filter_1) != HAL_OK) {
		User_Error_Handler(UERR_CAN_FILTER_CONFIG, 1);
	}

	if (HAL_CAN_Start(_usr_can) != HAL_OK)
	{
		User_Error_Handler(UERR_CAN_START, 1);
	}

//	if (HAL_CAN_ActivateNotification(_usr_can, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
//	{
//		User_Error_Handler(UERR_CAN_NOTIFY_ACTIVATE, 0); // will be fatal when actually using interrupts
//	}
}

void _Reset_Outputs() {
	HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_Red_GPIO_Port, LED2_Red_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_RESET);
}

void User_Timer_Callback(TIM_HandleTypeDef *timer) {
	_timer_counters.can_msg++;
	_timer_counters.wiper_tick++;
	if (_timer_counters.can_msg >= CAN_INTERVAL) {
		user_flags.send_can = SET;
		_timer_counters.can_msg = 0;
	}
	if (_timer_counters.wiper_tick >= WIPER_INTERVAL) {
		user_flags.wiper_tick = SET;
		_timer_counters.wiper_tick = 0;
	}
}

void User_ADC_Callback(ADC_HandleTypeDef *adc) {
	throttle_value.adc = HAL_ADC_GetValue(adc);
	HAL_ADC_Start_IT(adc);
}

void _Reset_Variables() {
	vcu_state.A.bits = 0b00000000;
	vcu_state.B.bits = 0b00000000;
	stw_state.A.bits = 0b00000000;
	stw_state.B.bits = 0b00000000;
	stw_state.C.bits = 0b00000000;
	stw_state.D.bits = 0b00000000;
	user_flags.receive_can = RESET;
	user_flags.send_can = RESET;
	user_flags.wiper_tick = RESET;
	wheel_rpm = 0;
	drive_state.current = D_NEUTRAL;
	drive_state.prev = D_NEUTRAL;
	throttle_value.current = POT_ZERO;
	throttle_value.prev = POT_ZERO;
	throttle_value.ema = POT_ZERO;
	_rate_limiter.current = 0;
	_rate_limiter.prev = 0;
	_wiper_state.running = RESET;
	_wiper_state.step = 0;
	_timer_counters.can_msg = 0;
}

void User_Init(ADC_HandleTypeDef *adc_ptr, CAN_HandleTypeDef *can_ptr, TIM_HandleTypeDef *wiper_pwm_ptr, UART_HandleTypeDef *uart_ptr) {
	_usr_throttle_adc = adc_ptr;
	_usr_can = can_ptr;
	_usr_wiper_pwm = wiper_pwm_ptr;
	_usr_uart = uart_ptr;

	_Reset_Outputs();
	_Reset_Variables();
	_Init_CAN();

	HAL_ADC_Start_IT(adc_ptr);

	HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_SET);
}

void User_Loop() {
	_Read_GPIO_Inputs();

	HAL_GPIO_WritePin(Brake_light_GPIO_Port, Brake_light_Pin, vcu_state.A.BRAKE);

// TODO: fix CAN interrupt not working :c
//	if (user_flags.receive_can == SET) {
		_Receive_CAN();
//		if (HAL_CAN_GetRxFifoFillLevel(_usr_can, CAN_RX_FIFO0) == 0) {
//			user_flags.receive_can = RESET;
//		}
//	}

	if (user_flags.send_can == SET) {
		_Pot_Filter();
		_Send_VCU_State_CAN();

		if (vcu_state.A.MC_OW == RESET && vcu_state.A.AUTONOMOUS == RESET) {
			_Update_Drive_State();
			_Send_MC_Command_CAN(_Calculate_MC_Ref());
		}

		HAL_GPIO_TogglePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin);
		user_flags.send_can = RESET;
	}

	if (user_flags.wiper_tick == SET) {
		_Wiper_Tick();
		user_flags.wiper_tick = RESET;
		HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_RESET);
	}
}
