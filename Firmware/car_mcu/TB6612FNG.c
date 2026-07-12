/*
 * TB6612FNG.c
 *
 *  Created on: Jun 15, 2026
 */


// the TB6612FNG code was taken from [https://www.youtube.com/watch?v=zWYYvtU8Wt4] and modified to fit the joystick module.


#include "stm32f4xx_hal.h"
#include "TB6612FNG.h"
#include "main.h"

extern TIM_HandleTypeDef htim12;

void Motors(uint32_t Joystick_val) {

	uint16_t duty_cycle = 0;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1); // STBY off, working

	if (Joystick_val < 2000) {
		//backwards

		duty_cycle = ((2000 - Joystick_val) * 1000) / 2000;

		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, duty_cycle);
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, duty_cycle);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1); // AIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0); // AIN2 high (bwd)

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // BIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1); // BIN2 high (bwd)
	}

	if ((Joystick_val >= 2000) && (Joystick_val <= 2200)) {
		//motors off

		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0); // stops timer
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0); // stop
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0); // stop
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);

	}

	if (Joystick_val > 2200) {
		//forwards

		duty_cycle = ((Joystick_val - 2200) * 1000) / (4095 - 2200);

		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, duty_cycle);
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, duty_cycle);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0); // AIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1); // AIN2 high (fwd)

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1); // BIN1 low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0); // BIN2 high (fwd)
	}
}

