/*
 * utils.c
 *
 *  Created on: 29 мая 2020 г.
 *      Author: ADiKo
 */

#include "utils.h"
#include "globals.h"
#include "structures.h"

void all_led_off(void){
	LED_STEP_1_OFF;
	LED_STEP_2_OFF;
	LED_STEP_3_OFF;
	LED_STEP_4_OFF;
	LED_STEP_5_OFF;

	LED_ERR_1_OFF;
	LED_ERR_2_OFF;
	LED_ERR_3_OFF;
	LED_ERR_4_OFF;
	LED_ERR_5_OFF;
}

uint8_t calculate_test_params(uint8_t start){
	uint8_t legalStep = 0;
	uint8_t i = 0;
	if (start >= 0 && start < 6){
		for(i = start; i < 5; i++){
			if (	(testProgram.testPressure[i] > 0) &&
					(testProgram.testPressure[i] < 120) &&
					(testProgram.testTime[i] > 0) &&
					(testProgram.testDiffPressure[i] > 0) &&
					(testProgram.testDiffPressure[i] < testProgram.testPressure[i])){
				legalStep = i + 1;

				testingParams.testDiffPressure = bar_to_adc_diff(testProgram.testDiffPressure[i]);
				testingParams.testPressure = bar_to_adc(testProgram.testPressure[i]);
				testingParams.testTimeMS = testProgram.testTime[i] * 60 * 1000;
				return legalStep;
			}
		}
	}
	return legalStep;
}

float adc_to_bar(uint16_t val){
	float valInBar = 0;
	float k = 20.0 / (3350.0 - 313.0); //calc 373.0
	float b = 20 - k * 3350.0;

	valInBar = k * (float)val + b;

	if (valInBar > 20.0) return 20.0;
	else if (valInBar < -1.0) return -1.0;
	else if (valInBar < 0.0) return 0.0;
	else return valInBar;
}

uint16_t bar_to_adc(uint16_t bar){
	float valInBar = (float)bar / 10.0;
	float k = (3350.0 - 313.0) / 20.0;
	float b = 3350.0 - k * 20.0;
	uint16_t adcVal = 0;

	adcVal = (uint16_t)(k * valInBar + b);

	if (adcVal > 3350) return 3350;
	else if (adcVal < 313) return 313;
	else return adcVal;
}

uint16_t bar_to_adc_diff(uint16_t bar){
	float valInBar = (float)bar / 10.0;
	float k = (3350.0 - 313.0) / 20.0;
	uint16_t adcVal = 0;

	adcVal = (uint16_t)(k * valInBar);

	if (adcVal > 3350) return 3350;
	else if (adcVal < 313) return 313;
	else return adcVal;
}
