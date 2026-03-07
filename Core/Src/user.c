/*
 * user.c
 *
 *	Made for Shell Eco Marathon by SZEnergy team
 *
 *  Created on: Mar 5, 2026
 *      Author: varma02@Github
 */

#include "user.h"

ADC_HandleTypeDef *pot_adc;
CAN_HandleTypeDef *can;
TIM_HandleTypeDef *wiper_pwm;

struct {
	VCU_STATE_A A;
	VCU_STATE_B B;
} vcu_state;


void User_Error_Handler(USER_ERROR err) {
	while (1) {
		HAL_GPIO_TogglePin(LED2_Red_GPIO_Port, LED2_Red_Pin);
		HAL_Delay(250);
	}
}

void User_Init(ADC_HandleTypeDef *adc_ptr, CAN_HandleTypeDef *can_ptr, TIM_HandleTypeDef *wiper_pwm_ptr) {
	pot_adc = adc_ptr;
	can_ptr = can_ptr;
	wiper_pwm = wiper_pwm_ptr;

	HAL_GPIO_WritePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_Red_GPIO_Port, LED2_Red_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_Green_GPIO_Port, LED3_Green_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Wiper_DCDC_enable_GPIO_Port, Wiper_DCDC_enable_Pin, GPIO_PIN_RESET);
}

void _test_can_msg() {
	CAN_TxHeaderTypeDef header;
	uint8_t data[6];
	uint32_t mailbox;

	header.IDE = CAN_ID_STD;
	header.StdId = 0x129;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 6;

	data[0] = vcu_state.A.bits;
	data[1] = vcu_state.B.bits;

	if (HAL_CAN_AddTxMessage(can, &header, data, &mailbox) != HAL_OK) {
//		User_Error_Handler(UER);
		return;
	}
}

void Read_GPIO_Inputs() {
	vcu_state.A.AUTO           = HAL_GPIO_ReadPin(Autonomous_switch_GPIO_Port, Autonomous_switch_Pin);
	vcu_state.A.HAZARD         = HAL_GPIO_ReadPin(Hazard_switch_GPIO_Port, Hazard_switch_Pin);
	vcu_state.A.LIGHTS_ENABLE  = HAL_GPIO_ReadPin(Lights_enable_switch_GPIO_Port, Lights_enable_switch_Pin);
	vcu_state.A.MC_OW          = HAL_GPIO_ReadPin(Motorcontrol_override_switch_GPIO_Port, Motorcontrol_override_switch_Pin);
	vcu_state.A.WIPER          = HAL_GPIO_ReadPin(Wiper_switch_GPIO_Port, Wiper_switch_Pin);
	vcu_state.A.HEADLIGHT      = HAL_GPIO_ReadPin(Headlight_switch_GPIO_Port, Headlight_switch_Pin);
	vcu_state.A.BRAKE          = HAL_GPIO_ReadPin(Brake_pedal_input_GPIO_Port, Brake_pedal_input_Pin);
}

void User_Loop() {
	HAL_GPIO_TogglePin(LED1_Yellow_GPIO_Port, LED1_Yellow_Pin);
	HAL_Delay(50);

	HAL_GPIO_TogglePin(LED3_Green_GPIO_Port, LED2_Red_Pin);
	HAL_Delay(50);

	HAL_GPIO_TogglePin(LED3_Green_GPIO_Port, LED3_Green_Pin);
	Read_GPIO_Inputs();
	_test_can_msg();
	HAL_Delay(50);

	HAL_GPIO_TogglePin(LED4_Blue_GPIO_Port, LED4_Blue_Pin);
	HAL_Delay(50);
}
