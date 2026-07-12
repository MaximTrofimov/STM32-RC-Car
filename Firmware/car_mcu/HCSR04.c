/*
 * HCSR04.c
 *
 *  Created on: Jun 15, 2026
 */



// the HCSR04 functions were originally taken from an article and modified to fit three HCSR04 sensors.
// [https://controllerstech.com/hcsr04-ultrasonic-sensor-and-stm32/]

#include "stm32f4xx_hal.h"
#include "HCSR04.h"

#define TRIG_PORT_F GPIOC
#define TRIG_PIN_F GPIO_PIN_1

#define TRIG_PORT_L GPIOA
#define TRIG_PIN_L GPIO_PIN_10

#define TRIG_PORT_R GPIOC
#define TRIG_PIN_R GPIO_PIN_10

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

extern volatile float distance_front;
extern volatile float distance_right;
extern volatile float distance_left;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart2;

//use to keep TRIG high for 10us
void delay(uint16_t time) {
	__HAL_TIM_SET_COUNTER(&htim2, 0);	// starts the counter at 0
	while (__HAL_TIM_GET_COUNTER(&htim2) < time)
		;	// while less than 10us do nothing
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM8 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		//front

		if (Is_First_Captured_F == 0) // if the first value is not captured
				{
			IC_Val1_F = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured_F = 1;  // set the first captured as true
			// Now change the polarity to falling edge for hardware, polarity determines which edge triggers the interrupt
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured_F == 1) // if the first is already captured
				{
			IC_Val2_F = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2_F > IC_Val1_F) {
				Difference_F = IC_Val2_F - IC_Val1_F;
			}

			else if (IC_Val1_F > IC_Val2_F) { // counter overflows
				Difference_F = (0xffff - IC_Val1_F) + IC_Val2_F;
			}

			distance_front = (Difference_F * .034) / 2; // speed of sound divided by two (cm/us)
			Is_First_Captured_F = 0; // set it back to false

			// set polarity to rising edge for hardware, polarity determines which edge triggers the interrupt
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim8, TIM_IT_CC1);
		}

	} else if (htim->Instance == TIM8
			&& htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
		//right

		if (Is_First_Captured_R == 0) // if the first value is not captured
				{
			IC_Val1_R = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4); // read the first value
			Is_First_Captured_R = 1;  // set the first captured as true
			// Now change the polarity to falling edge for hardware, polarity determines which edge triggers the interrupt
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured_R == 1) // if the first is already captured
				{
			IC_Val2_R = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4); // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2_R > IC_Val1_R) {
				Difference_R = IC_Val2_R - IC_Val1_R;
			}

			else if (IC_Val1_R > IC_Val2_R) { // counter overflows
				Difference_R = (0xffff - IC_Val1_R) + IC_Val2_R;
			}

			distance_right = (Difference_R * .034) / 2; // speed of sound divided by two
			Is_First_Captured_R = 0; // set it back to false

			// set polarity to rising edge for hardware, polarity determines which edge triggers the interrupt
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim8, TIM_IT_CC4);
		}

	} else if (htim->Instance == TIM1
			&& htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		//left

		if (Is_First_Captured_L == 0) // if the first value is not captured
				{
			IC_Val1_L = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // read the first value
			Is_First_Captured_L = 1;  // set the first captured as true
			// Now change the polarity to falling edge for hardware, polarity determines which edge triggers the interrupt
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured_L == 1) // if the first is already captured
				{
			IC_Val2_L = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2_L > IC_Val1_L) {
				Difference_L = IC_Val2_L - IC_Val1_L;
			}

			else if (IC_Val1_L > IC_Val2_L) { // counter overflows
				Difference_L = (0xffff - IC_Val1_L) + IC_Val2_L;
			}

			distance_left = (Difference_L * .034) / 2; // speed of sound divided by two
			Is_First_Captured_L = 0; // set it back to false

			// set polarity to rising edge for hardware, polarity determines which edge triggers the interrupt
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC2);
		}

	}

}

void HCSR04_ReadFront(void) { // GPIO_TypeDef is a struct where each member corresponds to one register
	HAL_GPIO_WritePin(TRIG_PORT_F, TRIG_PIN_F, GPIO_PIN_SET); // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT_F, TRIG_PIN_F, GPIO_PIN_RESET); // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim8, TIM_IT_CC1); // tells timer peripheral to start listening to echo pulse returning on echo pin
	// intentionally placed after the 10ms trig pulse so interrupt doesnt catch noise
}

void HCSR04_ReadRight(void) { // GPIO_TypeDef is a struct where each member corresponds to one register
	HAL_GPIO_WritePin(TRIG_PORT_R, TRIG_PIN_R, GPIO_PIN_SET); // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT_R, TRIG_PIN_R, GPIO_PIN_RESET); // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim8, TIM_IT_CC4);
}

void HCSR04_ReadLeft(void) { // GPIO_TypeDef is a struct where each member corresponds to one register
	HAL_GPIO_WritePin(TRIG_PORT_L, TRIG_PIN_L, GPIO_PIN_SET); // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT_L, TRIG_PIN_L, GPIO_PIN_RESET); // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC2);
}

