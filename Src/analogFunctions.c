/*
 * analogFunctions.c
 *
 *  Created on: 27 мая 2020 г.
 *      Author: ADiKo
 */

#include "analogFunctions.h"

#include "globals.h"
#include "structures.h"

#include "utils.h"

extern ADC_HandleTypeDef hadc1;

extern uint16_t adcRawData;
extern uint8_t symbolsToShow[2];

extern xSemaphoreHandle xNewPressureSemaphore;

void scanAnalogInput(void *arguments){
	uint16_t array[10] = {1};
	uint8_t pArray = 0;
	uint32_t sum = 0;
	uint8_t i = 0;
	StateMachine prevState = IDLE;

	for(;;){
		if (pArray == 10){
			pArray = 0;
			sum = 0;
			for (i = 0; i < 10; i++){
				sum += array[i];
			}
			controllerState.sensorData = (uint16_t)(sum / 10);

			controllerState.dataToSend[0] = controllerState.sensorData;

			controllerState.sensorDataBar = adc_to_bar(controllerState.sensorData);
			controllerState.valueToShow = (int16_t)(controllerState.sensorDataBar * 10.0);
			if (controllerState.valueToShow > 99) controllerState.valueToShow = 99;

			if (controllerState.valueToShow < 0){
				symbolsToShow[0] = 11;
				symbolsToShow[1] = 1;
			}
			else{
				symbolsToShow[0] = controllerState.valueToShow / 10;
				symbolsToShow[1] = controllerState.valueToShow % 10;
			}

			if (controllerState.state == TESTING || controllerState.state == ALIGNING) {
				//giving semaphore
				xSemaphoreGive(xNewPressureSemaphore);
				prevState = controllerState.state;
			}
			else if (prevState != controllerState.state && (prevState == TESTING || prevState == ALIGNING)){
				if (aligninThreadHandle != NULL){
					vTaskDelete(aligninThreadHandle);
					aligninThreadHandle = NULL;
				}
				if (testingTreadHandle != NULL){
					vTaskDelete(testingTreadHandle);
					testingTreadHandle == NULL;
				}
			}
		}
		else{
			array[pArray] = adcRawData;
			pArray++;
		}

		HAL_ADC_Start_IT(&hadc1);



		vTaskDelay(50);
	}
	vTaskDelete(NULL);
}
