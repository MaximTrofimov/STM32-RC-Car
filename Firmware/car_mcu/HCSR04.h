/*
 * HCSR04.h
 *
 *  Created on: Jun 15, 2026
 */

#ifndef INC_HCSR04_H_
#define INC_HCSR04_H_

void HCSR04_ReadFront(void);
void HCSR04_ReadRight(void);
void HCSR04_ReadLeft(void);
void delay(uint16_t time);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

#endif /* INC_HCSR04_H_ */
