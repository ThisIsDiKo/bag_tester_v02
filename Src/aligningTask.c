/*
 * aligningTask.c
 *
 *  Created on: 29 мая 2020 г.
 *      Author: ADiKo
 */

#include "aligningTask.h"
#include "testingTask.h"

#include "globals.h"
#include "structures.h"

extern xSemaphoreHandle xNewPressureSemaphore;


void xAligningTask(void *arguments){
	portBASE_TYPE xStatus;
	int32_t impTime = 0;
	int16_t startPressure = 0;
	int16_t deltaPressure = 0;
	uint8_t pressIsLower = 0;

	uint8_t currentTry = 0;

	uint32_t curImpCounter= 0;
	int32_t dImpCounter = 0;

	float impUpCoeff = 2.0;
	float impDownCoeff = -2.0;
	float impCoeff = 0.0;

	xStatus = xSemaphoreTake(xNewPressureSemaphore, portMAX_DELAY);
	for(;;){
		xStatus = xSemaphoreTake(xNewPressureSemaphore, portMAX_DELAY);
		if (xStatus == pdPASS){

			if (currentTry > MAX_ALIGNING_TRIES){

				//wait
				vTaskDelay(DELAY_AFTER_ALIGNING_MS);
				//create task
				testingParams.testStartPressure = controllerState.sensorData;
				testingParams.timePassed = 0;
				xTaskCreate(xTestingTask, "testing", 200, NULL, 1, NULL);
				//delete current task
				vTaskDelete(NULL);
				continue;
			}

			startPressure = controllerState.sensorData;
			deltaPressure = testingParams.testPressure - startPressure;
			deltaPressure = abs(deltaPressure);

			if (deltaPressure > PRESSURE_ACCURACY_THRESHOLD){
				if (testingParams.testPressure > startPressure){
					pressIsLower = 1;
				}
				else{
					pressIsLower = 0;
				}
			}
			else{
				currentTry = MAX_ALIGNING_TRIES + 1;
				continue;
			}

			if (pressIsLower){
				impTime = (int32_t)(impUpCoeff * (float) deltaPressure);
				if (impTime <= 0) impTime = 1000;
				else if (impTime > 5000) impTime = 5000;
				VALVE_UP_ON;
				controllerState.valveIsOpen = 1;
			}
			else{
				impTime = (int32_t)(impDownCoeff * (float) deltaPressure);
				if (impTime <= 0) impTime = 1000;
				else if (impTime > 5000) impTime = 5000;
				VALVE_DOWN_ON;
				controllerState.valveIsOpen = -1;
			}

			curImpCounter = xTaskGetTickCount();

			while(1){
				vTaskDelay(10);
				dImpCounter = xTaskGetTickCount() -  curImpCounter;
				if (dImpCounter > impTime){
					controllerState.valveIsOpen = 0;
					VALVE_UP_OFF;
					VALVE_DOWN_OFF;
					break;
				}
			}

			vTaskDelay(3000);

			deltaPressure = controllerState.sensorData - startPressure;
			impCoeff = (float)impTime / (float) deltaPressure;
			if (pressIsLower){
				if (impCoeff >= 0.0) impUpCoeff = impCoeff;
			}
			else{
				if (impCoeff <= 0.0) impDownCoeff = impCoeff;
			}

			deltaPressure = abs(deltaPressure);
			if (deltaPressure < PRESSURE_ACCURACY_THRESHOLD && impTime > 1000){
				controllerState.state = TEST_ERROR;
				vTaskDelete(NULL);
			}

			currentTry += 1;
		}
	}
	vTaskDelete(NULL);
}
