/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Headlight_switch_Pin GPIO_PIN_13
#define Headlight_switch_GPIO_Port GPIOC
#define Lights_enable_switch_Pin GPIO_PIN_3
#define Lights_enable_switch_GPIO_Port GPIOC
#define Throttle_pedal_ADC_Pin GPIO_PIN_0
#define Throttle_pedal_ADC_GPIO_Port GPIOA
#define LED1_Yellow_Pin GPIO_PIN_1
#define LED1_Yellow_GPIO_Port GPIOA
#define LED2_Red_Pin GPIO_PIN_2
#define LED2_Red_GPIO_Port GPIOA
#define LED3_Green_Pin GPIO_PIN_3
#define LED3_Green_GPIO_Port GPIOA
#define LED4_Blue_Pin GPIO_PIN_4
#define LED4_Blue_GPIO_Port GPIOA
#define Autonomous_switch_Pin GPIO_PIN_4
#define Autonomous_switch_GPIO_Port GPIOC
#define Motorcontrol_override_switch_Pin GPIO_PIN_5
#define Motorcontrol_override_switch_GPIO_Port GPIOC
#define Wiper_switch_Pin GPIO_PIN_0
#define Wiper_switch_GPIO_Port GPIOB
#define Brake_light_Pin GPIO_PIN_10
#define Brake_light_GPIO_Port GPIOB
#define Clutch_pedal_input_Pin GPIO_PIN_8
#define Clutch_pedal_input_GPIO_Port GPIOA
#define Brake_pedal_input_Pin GPIO_PIN_9
#define Brake_pedal_input_GPIO_Port GPIOA
#define Debug_Out_Pin GPIO_PIN_12
#define Debug_Out_GPIO_Port GPIOC
#define Wiper_PWM_Pin GPIO_PIN_4
#define Wiper_PWM_GPIO_Port GPIOB
#define Hazard_switch_Pin GPIO_PIN_5
#define Hazard_switch_GPIO_Port GPIOB
#define Wiper_DCDC_enable_Pin GPIO_PIN_8
#define Wiper_DCDC_enable_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
