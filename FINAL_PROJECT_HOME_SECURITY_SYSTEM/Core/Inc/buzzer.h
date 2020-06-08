/*
 * This module contains methods to handle with a buzzer, represented with a structure holding:
 * 		a pointer to the TIM_HandleTypeDef structure representing the timer generating the PWM signal
 * 			that controls the sound of the buzzer
 * 		the current duty cycle in [0, 1[
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <stdlib.h>
#include <assert.h>

#include "stm32f4xx_hal.h"
#include "utils.h"
#include "bool.h"
#include "tim.h"

#define BUZZER_MAX_PULSE		(1000)
#define BUZZER_BEEP				(0.1 * BUZZER_MAX_PULSE)
#define BUZZER_SHORT_PULSE		(0.2 * BUZZER_MAX_PULSE)
#define BUZZER_MEDIUM_PULSE		(0.6 * BUZZER_MAX_PULSE)
#define BUZZER_LONG_PULSE		(0.9 * BUZZER_MAX_PULSE)

// the duty cycle is in [0, 1[ so it's stored in a float variable
typedef float TDutyCycle;

// the integer value of the duty cycle is in [0, MAX_VALUE[ so it's stored in an integer variable
typedef uint32_t TPulse;

/*
 * @brief	This struct represents the buzzer,
 * 			encapsulating the timer interface used to generate the PWM signal that control the buzzer.
 * @param	htim				pointer to the TIM_HandleTypeDef structure representing the timer interface
 * 								used to generate the PWM signal that control the buzzer
 * @param	current_duty_cycle	the current duty cycle
 */
typedef struct {
	TIM_HandleTypeDef *htim;
	uint32_t Channel;
	TPulse pulse;
	TPulse previous_pulse;
	TDutyCycle duty_cycle;
	bool single_pulse_mode;
} TBuzzer;

/*
 * @fn		TBuzzer* buzzer_init(TIM_HandleTypeDef *htim, uint32_t Channel)
 * @brief	Instantiates a buzzer
 * @param	htim		pointer to the TIM_HandleTypeDef structure representing the timer interface
 * 						used to generate the PWM signal that control the buzzer
 * @param	Channel		the timer channel holding the PWM signal
 * @retval	pointer to the TBuzzer structure representing the buzzer
 */
TBuzzer* buzzer_init(TIM_HandleTypeDef *htim, uint32_t Channel);

/*
 * @fn		TDutyCycle get_duty_cycle(TBuzzer *buzzer)
 * @brief	Returns the duty cycle of a buzzer
 * @param	buzzer		pointer to the TBuzzer structure representing the buzzer
 * @retval	current duty cycle of the buzzer
 */
TDutyCycle buzzer_get_duty_cycle(TBuzzer *buzzer);

/*
 * @fn		TValue get_duty_cycle_value(TBuzzer *buzzer)
 * @brief	Returns the integer duty cycle value of a buzzer
 * @param	buzzer		pointer to the TBuzzer structure representing the buzzer
 * @retval	current integer duty cycle value of the buzzer
 */
TPulse buzzer_get_pulse(TBuzzer *buzzer);


/*
 * @fn		void set_duty_cycle(TBuzzer *buzzer, const TDutyCycle duty_cycle)
 * @brief	Sets the duty cycle of a buzzer to a specified value in [0, 1[
 * @param	buzzer		pointer to the TBuzzer structure representing the buzzer
 * @param	dutyCycle	duty cycle to set
 */
void buzzer_set_duty_cycle(TBuzzer *buzzer, const TDutyCycle duty_cycle);

/*
 * @fn		void set_duty_cycle_to_value(TBuzzer *buzzer, const TValue value)
 * @brief	Sets the duty cycle of a buzzer to a value corresponding to an integer value in [0, MAX_VALUE[
 * 			The conversion formula is dutyCycle = value / MAX_VALUE
 * 			For example, if value is 100, then the duty cycle will be 100 / 255 = 0,3922
 * @param	buzzer		pointer to the TBuzzer structure representing the buzzer
 * @param	value		integer duty cycle value to set
 */
void buzzer_set_pulse(TBuzzer *buzzer, const TPulse pulse);

void buzzer_play_duty_cycle(TBuzzer *buzzer, TDutyCycle duty_cycle);

void buzzer_play_pulse(TBuzzer *buzzer, TPulse pulse);

void buzzer_play_beep(TBuzzer *buzzer);

void buzzer_stop(TBuzzer *buzzer);

void buzzer_increase_pulse(TBuzzer *buzzer, TPulse next_pulse);

void buzzer_decrease_pulse(TBuzzer *buzzer, TPulse previous_pulse);

void buzzer_change_pulse(TBuzzer *buzzer, uint16_t pulse);

/*
 * @fn		static TValue get_value_of_duty_cycle(const TDutyCycle duty_cycle)
 * @brief	Converts a duty cycle float value in [0,1[ to the corresponding interger value in [0, MAX_VALUE[
 * @param	dutyCycle	duty cycle value to convert
 * @retval	integer value corresponding to duty_cycle
 */
static TPulse get_pulse_of_duty_cycle(const TDutyCycle duty_cycle) {
	return (TPulse) map(duty_cycle, 0.0, 1.0, 0, BUZZER_MAX_PULSE);
}

/*
 * @fn		static TDutyCycle get_duty_cycle_of_value(const TValue value)
 * @brief	Converts a duty cycle integer value in [0, MAX_VALUE[ to the corresponding float value in [0,1[
 * @param	value	integer duty cycle value to convert
 * @retval	float duty cycle value corresponding to value
 */
static TDutyCycle get_duty_cycle_of_pulse(const TPulse pulse) {
	return map(pulse, 0, BUZZER_MAX_PULSE, 0.0, 1.0);
}

TPulse buzzer_beep();

TPulse buzzer_short_pulse();

TPulse buzzer_medium_pulse();

TPulse buzzer_long_pulse();

#endif /* INC_BUZZER_H_ */

