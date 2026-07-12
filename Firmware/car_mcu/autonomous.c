/*
 * autonomous.c
 *
 *  Created on: Jun 17, 2026
 */

#include "stm32f4xx_hal.h"
#include "autonomous.h"
#include "HCSR04.h"
#include <stdio.h>
#include "main.h"

#define TRIG_PORT_F GPIOC
#define TRIG_PIN_F GPIO_PIN_1

#define TRIG_PORT_L GPIOA
#define TRIG_PIN_L GPIO_PIN_10

#define TRIG_PORT_R GPIOC
#define TRIG_PIN_R GPIO_PIN_10

extern volatile float distance_front;
extern volatile float distance_right;
extern volatile float distance_left;

extern volatile uint32_t IC_Val1_F;
extern volatile uint32_t IC_Val2_F;
extern volatile uint32_t Difference_F;
extern volatile uint8_t Is_First_Captured_F;

extern volatile uint32_t IC_Val1_R;
extern volatile uint32_t IC_Val2_R;
extern volatile uint32_t Difference_R;
extern volatile uint8_t Is_First_Captured_R;

extern volatile uint32_t IC_Val1_L;
extern volatile uint32_t IC_Val2_L;
extern volatile uint32_t Difference_L;
extern volatile uint8_t Is_First_Captured_L;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim12;

extern UART_HandleTypeDef huart2;

void Autonomous_mode(void) {

	// turn neutral
	htim3.Instance->CCR3 = 75;
	HAL_Delay(100);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1); // STBY off, working

	// front sensor
	HCSR04_ReadFront();
	HAL_Delay(20);

	// go forward
	__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 500);
	__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 500);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);  // AIN2 high (fwd)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1); // BIN1 low
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 high (fwd)

	if (distance_front < 30.0) {

		// stop motors
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 low

		HAL_Delay(250);

		// go back for a few seconds
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 500);
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 500);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);  // AIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 high (bwd)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1); // BIN2 high (bwd)

		HAL_Delay(500);

		// stop motors
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 low

		HAL_Delay(250);

		// check left
		HCSR04_ReadLeft();
		HAL_Delay(20);
		// check right
		HCSR04_ReadRight();
		HAL_Delay(20);

		if (distance_left > distance_right) {
			// turn left
			htim3.Instance->CCR3 = 75 - 14;

			// go forward
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 500);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 500);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);  // AIN2 high (fwd)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 high (fwd)

			HAL_Delay(200); // time for turn

			// stop motors
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 low

			HAL_Delay(250);

			// turn right
			htim3.Instance->CCR3 = 75 + 14;
			// go back for a few seconds
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 500);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 500);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 high (bwd)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1); // BIN2 high (bwd)

			HAL_Delay(500);

			// stop motors
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 low

			HAL_Delay(250);

		}

		if (distance_right > distance_left) {
			// turn right
			htim3.Instance->CCR3 = 75 + 14;

			// go forward
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 500);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 500);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);  // AIN2 high (fwd)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 high (fwd)

			HAL_Delay(200); // time for turn

			// stop motors
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 low

			HAL_Delay(250);

			// turn left
			htim3.Instance->CCR3 = 75 - 14;
			// go back for a few seconds
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 500);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 500);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 high (bwd)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1); // BIN2 high (bwd)

			HAL_Delay(500);

			// stop motors
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);  // AIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);  // AIN2 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 low

			HAL_Delay(250);
		}
	}
}
