/*
 * testingTask.c
 *
 *  Created on: 29 мая 2020 г.
 *      Author: ADiKo
 */


/*
 * aligningTask.c
 *
 *  Created on: 29 мая 2020 г.
 *      Author: ADiKo
 */
#include "testingTask.h"
#include "aligningTask.h"

#include "utils.h"

#include "globals.h"
#include "structures.h"

extern xSemaphoreHandle xNewPressureSemaphore;


void xTestingTask(void *arguments){
	portBASE_TYPE xStatus;
	uint8_t badPressTicksCounter = 0;
	int16_t deltaPressure = 0;

	xStatus = xSemaphoreTake(xNewPressureSemaphore, portMAX_DELAY);
	for(;;){
		xStatus = xSemaphoreTake(xNewPressureSemaphore, portMAX_DELAY);
		if (xStatus == pdPASS){

			vTaskDelay(testingParams.checkingDelayMs);

			deltaPressure = testingParams.testStartPressure - controllerState.sensorData;

			if (deltaPressure > testingParams.testDiffPressure){
				badPressTicksCounter += 1;

				if (badPressTicksCounter > testingParams.maxNumberOfBadPressure){
					all_led_off();
					controllerState.state = STEP_ERROR;
					vTaskDelete(NULL);
				}
			}
			else{
				testingParams.timePassed += testingParams.checkingDelayMs;
				if (testingParams.timePassed > testingParams.testTimeMS){
					//check next test
					controllerState.currentStep = calculate_test_params(controllerState.currentStep);

					//call aligning task
					if (controllerState.currentStep > 0){
						all_led_off();
						controllerState.state = ALIGNING;
						xTaskCreate(xAligningTask, "aligning", 400, NULL, 1, &aligninThreadHandle);
					}
					else{
						all_led_off();
						controllerState.state = TEST_COMPLETE;
					}
					//delete curTask
					testingTreadHandle = NULL;
					vTaskDelete(NULL);
				}
			}
		}
	}
	vTaskDelete(NULL);
}
