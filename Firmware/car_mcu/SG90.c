/*
 * SG90.c
 *
 *  Created on: Jun 15, 2026
 */


// the SG90 code was taken from [https://controllerstech.com/servo-motor-with-stm32/] and modified to fit the joystick module.


#include "stm32f4xx_hal.h"
#include "SG90.h"

extern TIM_HandleTypeDef htim3;

void Servo(uint32_t Joystick_val) {

	if (Joystick_val < 1950) {
		// left
		uint16_t signal_width = (-1*(Joystick_val-1950) * 14) / 1950;

		htim3.Instance->CCR3 = 75 - signal_width;
	}

	if (Joystick_val > 2150) {
		// right
		uint16_t signal_width = ((Joystick_val - 2150) * 14) / 1950;

		htim3.Instance->CCR3 = 75 + signal_width;
	}

	if ((Joystick_val >= 1950) && (Joystick_val <= 2150)) {
		// straight
		htim3.Instance->CCR3 = 75;
	}

}


