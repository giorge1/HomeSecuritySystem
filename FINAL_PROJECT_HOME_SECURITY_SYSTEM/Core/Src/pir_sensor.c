/*
 * pir_sensor.c
 *
 *  Created on: May 31, 2020
 *      Author: gioam
 */

#include "pir_sensor.h"

/**
 * @brief Initialize a pir sensor.
 * @param pir: the structure which will hold the sensor
 * @param delay: the delay in second
 * @param alarm_duration: for how much time in seconds the sensor will be in alarm state
 * @param irq: the irq corresponding to the port of the pir sensor
 * @param port: the port which the sensor is connected to
 * @param pin: the port which the sensor is connected to
 * @param timer: the timer dedicated to the sensor
 * @return None
 */
void PIR_sensor_Init(TPIR_sensor *pir, uint8_t delay, uint8_t alarm_duration,
		IRQn_Type irq, GPIO_TypeDef *port, uint16_t pin,
		TIM_HandleTypeDef *timer, TBuzzer *buzzer) {

	pir->state = ALARM_STATE_INACTIVE;
	pir->delay = delay;
	pir->remaining_delay = delay;
	pir->alarm_duration = alarm_duration;
	pir->irq = irq;
	pir->pin = pin;
	pir->port = port;
	pir->timer = timer;
	pir->buzzer = buzzer;
	return;
}

/**
 * @brief Disable and set as inactive a pir sensor. NOTE: also the irq and the timer will be disabled
 * @param pir: the structure of the pin to deactivate
 * @return None
 */
void PIR_sensor_deactivate(TPIR_sensor *pir) {
	PIR_Change_State(pir, ALARM_STATE_INACTIVE);
}

/**
 * @brief Reinitialize and set as active a pir sensor using the previous configuration
 * @param pir: the structure which will hold the sensor
 * @return None
 */
void PIR_sensor_activate(TPIR_sensor *pir) {
	PIR_Change_State(pir, ALARM_STATE_ACTIVE);
	return;
}

/**
 * @brief ISR of the pir_sensor. This should be called both on rising and falling edge
 * @param pir: the structure of the pin which has triggered the interruption
 * @return None
 */
void PIR_Sensor_handler(TPIR_sensor *pir) {
	__HAL_GPIO_EXTI_CLEAR_IT(pir->pin);

	//If the interrupt is on the falling edge, then check the state of the sensor
	if (HAL_GPIO_ReadPin(pir->port, pir->pin) == GPIO_PIN_RESET) {
		if (pir->state == ALARM_STATE_DELAYED) {
			//if the sensor was on delayed state, stop the timer and set the sensor back to active
			PIR_Change_State(pir, ALARM_STATE_ACTIVE);
		}
		//in any other case, ignore the interrupt. There are 3 other possible states
		// active: it is not possible since if it is falling, it has raised, so it was already in delayed
		// inactive: impossible, since the interrupt was deactivated
		// alarm: ignore it, the sensor is already alarmed
		return;
	}

	// if it is rising, but it is already alarmed, don't do anything
	if (pir->state == ALARM_STATE_ALARMED) {
		return;
	}

	PIR_Change_State(pir, ALARM_STATE_DELAYED);

}

/**
 * @brief ISR of the associated timer. This should be called both on rising and falling edge
 * @param pir: the structure of the pin which is currently on alarm or delayed state
 * @return None
 */
void PIR_Time_elapsed(TPIR_sensor *pir) {

	//Decrease the remaining delay
	--(pir->remaining_delay);
	if (pir->remaining_delay == 0U) {
		// if the delay has passed, start the alarm
		PIR_Change_State(pir, ALARM_STATE_ALARMED);
		return;
	}
	//decreasing remaining delay will cause an under flow, so sum it to the duration and wait until it the overflow of the sum is zero
	uint8_t t = pir->remaining_delay + pir->alarm_duration;
	if (t == 0) {
		PIR_Change_State(pir, ALARM_STATE_ACTIVE);
	}
}

