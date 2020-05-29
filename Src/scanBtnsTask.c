/*
 * scanBtnsTask.c
 *
 *  Created on: 27 мая 2020 г.
 *      Author: ADiKo
 */


#include "scanBtnsTask.h"
#include "globals.h"
#include "structures.h"

#include "utils.h"

#include "aligningTask.h"

void scanBtnsTask(void *arguments){
	uint8_t prevBtn1State = 0;
	uint8_t prevBtn2State = 0;
	uint8_t i = 0;
	for(;;){
		if(HAL_GPIO_ReadPin(BTN_UP_PORT, BTN_UP_PIN))
			VALVE_UP_ON;
		else{
			if (controllerState.valveIsOpen != 1){
				VALVE_UP_OFF;
			}
		}

		if(HAL_GPIO_ReadPin(BTN_DOWN_PORT, BTN_DOWN_PIN))
			VALVE_DOWN_ON;
		else{
			if (controllerState.valveIsOpen != -1){
				VALVE_DOWN_OFF;
			}
		}

		if(HAL_GPIO_ReadPin(BTN_1_PORT, BTN_1_PIN) != prevBtn1State){
			vTaskDelay(2);
			if(HAL_GPIO_ReadPin(BTN_1_PORT, BTN_1_PIN) != prevBtn1State){
				prevBtn1State = HAL_GPIO_ReadPin(BTN_1_PORT, BTN_1_PIN);

				if (prevBtn1State){
					switch(controllerState.state){
						case IDLE:{
							if (controllerState.sensorData == 0){
								controllerState.state = TEST_ERROR;
								break;
							}

							controllerState.currentStep = 0;

							controllerState.currentStep = calculate_test_params(controllerState.currentStep);

							if (controllerState.currentStep > 0){
								all_led_off();
								controllerState.state = TESTING;
								//create aligning task
								xTaskCreate(xAligningTask, "aligning", 400, NULL, 1, NULL);
							}
							else{
								all_led_off();
								controllerState.state = TEST_ERROR;
							}
							break;
						}
						case TESTING:{
							all_led_off();
							controllerState.state = IDLE;
							controllerState.currentStep = 0;
							break;
						}
						case TEST_COMPLETE:{
							all_led_off();
							controllerState.state = IDLE;
							controllerState.currentStep = 0;
							break;
						}
						case TEST_ERROR:{
							all_led_off();
							controllerState.state = IDLE;
							controllerState.currentStep = 0;
							break;
						}
						case STEP_ERROR:{
							all_led_off();
							controllerState.state = IDLE;
							controllerState.currentStep = 0;
							break;
						}
						default:{
							break;
						}
					}
				}
			}

		}
		vTaskDelay(5);
	}
	vTaskDelete(NULL);
}
