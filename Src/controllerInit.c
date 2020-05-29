/*
 * controllerInit.c
 *
 *  Created on: 27 мая 2020 г.
 *      Author: ADiKo
 */
#include "globals.h"
#include "structures.h"
#include "scanBtnsTask.h"
#include "analogFunctions.h"
#include "modbusPollTask.h"

#include "mb.h"
#include "mbport.h"

extern uint8_t debugDigit;
extern UART_HandleTypeDef huart1;
//PFP
void indicationTask(void *arguments);
void idle_led_blynk(uint8_t i);
void testing_led_blynk(uint8_t curStep);
void step_error_led_blynk(uint8_t curStep);

xSemaphoreHandle xNewPressureSemaphore;

uint8_t get_modbus_address(){
	uint8_t addr = 0;

	if (!HAL_GPIO_ReadPin(ADDR_0_PORT, ADDR_0_PIN)){
		addr |= (1 << 0);
	}
	if (!HAL_GPIO_ReadPin(ADDR_1_PORT, ADDR_1_PIN)){
		addr |= (1 << 1);
	}
	if (!HAL_GPIO_ReadPin(ADDR_2_PORT, ADDR_2_PIN)){
		addr |= (1 << 2);
	}
	if (!HAL_GPIO_ReadPin(ADDR_3_PORT, ADDR_3_PIN)){
		addr |= (1 << 3);
	}
	if (!HAL_GPIO_ReadPin(ADDR_4_PORT, ADDR_4_PIN)){
		addr |= (1 << 4);
	}
	if (!HAL_GPIO_ReadPin(ADDR_5_PORT, ADDR_5_PIN)){
		addr |= (1 << 5);
	}

	if (addr == 0) addr = 1;

	return addr;
}



void controller_init(){

	controllerState.modbusAddr = get_modbus_address();

	eMBErrorCode    eStatus;
	eStatus = eMBInit( MB_RTU, (UCHAR)controllerState.modbusAddr, 0, 38400, MB_PAR_NONE );
	//eStatus = eMBInit( MB_RTU, 17, 0, 38400, MB_PAR_NONE );

	eStatus = eMBEnable(  );

	vSemaphoreCreateBinary(xNewPressureSemaphore);

	testProgram.testPressure[0] = 21;
	testProgram.testTime[0] = 5;
	testProgram.testDiffPressure[0] = 11;
//
//	testProgram.testPressure[1] = 42;
//	testProgram.testTime[1] = 72;
//	testProgram.testDiffPressure[1] = 22;
//
//	testProgram.testPressure[2] = 73;
//	testProgram.testTime[2] = 23;
//	testProgram.testDiffPressure[2] = 43;
//
//	testProgram.testPressure[3] = 4;
//	testProgram.testTime[3] = 64;
//	testProgram.testDiffPressure[3] = 14;
//
//	testProgram.testPressure[4] = 25;
//	testProgram.testTime[4] = 5;
//	testProgram.testDiffPressure[4] = 15;

	xTaskCreate(indicationTask, "Blynk", 200, NULL, 1, NULL);
	xTaskCreate(scanBtnsTask, "Btns", 200, NULL, 1, NULL);
	xTaskCreate(scanAnalogInput, "analog", 200, NULL, 1, NULL);
	xTaskCreate(xModbusPollTask, "modbus", 300, NULL, 1, NULL);

}


void indicationTask(void *arguments){
	uint8_t i = 0;
	for(;;){
		switch(controllerState.state){
			case(IDLE):{
				idle_led_blynk(i);
				i++;
				if (i > 9) i = 0;
				break;
			}
			case(TESTING):{
				testing_led_blynk(controllerState.currentStep);
				break;
			}
			case(STEP_ERROR):{
				step_error_led_blynk(controllerState.currentStep);
				break;
			}
			case(TEST_COMPLETE):{
				HAL_GPIO_TogglePin(LED_STEP_1_PORT, LED_STEP_1_PIN);
				HAL_GPIO_TogglePin(LED_STEP_2_PORT, LED_STEP_2_PIN);
				HAL_GPIO_TogglePin(LED_STEP_3_PORT, LED_STEP_3_PIN);
				HAL_GPIO_TogglePin(LED_STEP_4_PORT, LED_STEP_4_PIN);
				HAL_GPIO_TogglePin(LED_STEP_5_PORT, LED_STEP_5_PIN);
				break;
			}
			case(TEST_ERROR):{
				HAL_GPIO_TogglePin(LED_ERR_1_PORT, LED_ERR_1_PIN);
				HAL_GPIO_TogglePin(LED_ERR_2_PORT, LED_ERR_2_PIN);
				HAL_GPIO_TogglePin(LED_ERR_3_PORT, LED_ERR_3_PIN);
				HAL_GPIO_TogglePin(LED_ERR_4_PORT, LED_ERR_4_PIN);
				HAL_GPIO_TogglePin(LED_ERR_5_PORT, LED_ERR_5_PIN);
				break;
			}
			default:{
				break;
			}
		}

		vTaskDelay(500);
	}
	vTaskDelete(NULL);
}

void testing_led_blynk(uint8_t curStep){
	switch(curStep){
		case(1):{
			HAL_GPIO_TogglePin(LED_STEP_1_PORT, LED_STEP_1_PIN);
			LED_STEP_2_OFF;
			LED_STEP_3_OFF;
			LED_STEP_4_OFF;
			LED_STEP_5_OFF;
			break;
		}
		case(2):{
			LED_STEP_1_ON;
			HAL_GPIO_TogglePin(LED_STEP_2_PORT, LED_STEP_2_PIN);
			LED_STEP_3_OFF;
			LED_STEP_4_OFF;
			LED_STEP_5_OFF;
			break;
		}
		case(3):{
			LED_STEP_1_ON;
			LED_STEP_2_ON;
			HAL_GPIO_TogglePin(LED_STEP_3_PORT, LED_STEP_3_PIN);
			LED_STEP_4_OFF;
			LED_STEP_5_OFF;
			break;
		}
		case(4):{
			LED_STEP_1_ON;
			LED_STEP_2_ON;
			LED_STEP_3_ON;
			HAL_GPIO_TogglePin(LED_STEP_4_PORT, LED_STEP_4_PIN);
			LED_STEP_5_OFF;
			break;
		}
		case(5):{
			LED_STEP_1_ON;
			LED_STEP_2_ON;
			LED_STEP_3_ON;
			LED_STEP_4_ON;
			HAL_GPIO_TogglePin(LED_STEP_5_PORT, LED_STEP_5_PIN);
			break;
		}
		default:{
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
	}
}

void step_error_led_blynk(uint8_t curStep){
	switch(curStep){
		case(1):{
			HAL_GPIO_TogglePin(LED_ERR_1_PORT, LED_ERR_1_PIN);
			LED_STEP_1_ON;
			break;
		}
		case(2):{
			HAL_GPIO_TogglePin(LED_ERR_2_PORT, LED_ERR_2_PIN);
			LED_STEP_2_ON;
			break;
		}
		case(3):{
			HAL_GPIO_TogglePin(LED_ERR_3_PORT, LED_ERR_3_PIN);
			LED_STEP_3_ON;
			break;
		}
		case(4):{
			HAL_GPIO_TogglePin(LED_ERR_4_PORT, LED_ERR_4_PIN);
			LED_STEP_4_ON;
			break;
		}
		case(5):{
			HAL_GPIO_TogglePin(LED_ERR_5_PORT, LED_ERR_5_PIN);
			LED_STEP_5_ON;
			break;
		}
		default:{
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
	}
}

void idle_led_blynk(uint8_t i){
	  switch(i){
		  case 0:{
			  LED_STEP_1_ON;
			  LED_ERR_5_OFF;
			  break;
		  }
		  case 1:{
			  LED_STEP_2_ON;
			  LED_STEP_1_OFF;
			  break;
		  }
		  case 2:{
			  LED_STEP_3_ON;
			  LED_STEP_2_OFF;
			  break;
		  }
		  case 3:{
			  LED_STEP_4_ON;
			  LED_STEP_3_OFF;
			  break;
		  }
		  case 4:{
			  LED_STEP_5_ON;
			  LED_STEP_4_OFF;
			  break;
		  }
		  case 5:{
			  LED_ERR_1_ON;
			  LED_STEP_5_OFF;
			  break;
		  }
		  case 6:{
			  LED_ERR_2_ON;
			  LED_ERR_1_OFF;
			  break;
		  }
		  case 7:{
			  LED_ERR_3_ON;
			  LED_ERR_2_OFF;
			  break;
		  }
		  case 8:{
			  LED_ERR_4_ON;
			  LED_ERR_3_OFF;
			  break;
		  }
		  case 9:{
			  LED_ERR_5_ON;
			  LED_ERR_4_OFF;
			  break;
		  }
		  default:{
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
			  break;
		  }
	  }
}
