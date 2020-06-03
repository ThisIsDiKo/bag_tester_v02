#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include "globals.h"

typedef enum{
	IDLE,
	TEST_COMPLETE,
	TEST_ERROR,
	TESTING,
	ALIGNING,
	STEP_ERROR,
} StateMachine;

typedef struct{
	uint8_t modbusAddr;
	uint16_t sensorData;
	float sensorDataBar;
	int16_t valueToShow;
	StateMachine state;
	uint8_t currentStep;
	uint8_t isTesting;
	uint8_t testFailed;
	uint16_t dataToSend[3];
	int8_t valveIsOpen;

}ControllerState;

typedef struct{
	uint16_t testPressure[5];
	uint16_t testTime[5];
	uint16_t testDiffPressure[5];
	uint16_t temp;
}ControllerTestProgram;

typedef struct{
	uint16_t testPressure;
	uint32_t testTimeMS;
	uint16_t testDiffPressure;
	uint16_t testStartPressure;
	uint64_t timePassed;
}TestingParams;

extern ControllerState controllerState;
extern ControllerTestProgram testProgram;
extern TestingParams testingParams;
extern uint8_t digitSegments[12];

extern TaskHandle_t aligninThreadHandle;
extern TaskHandle_t testingTreadHandle;

void all_led_off(void);

#endif
