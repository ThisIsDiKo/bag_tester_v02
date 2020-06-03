/*
 * structures.c
 *
 *  Created on: 27 мая 2020 г.
 *      Author: ADiKo
 */
#include "structures.h"
/*
 * ------------------
 * |DP G F E D C B A|
 * | 7 6 5 4 3 2 1 0|
 * ------------------
 */
uint8_t digitSegments[12] = {0b00111111,//0
							 0b00000110,//1
							 0b01011011,//2
							 0b01001111,//3
							 0b01100110,//4
							 0b01101101,//5
							 0b01111101,//6
							 0b00000111,//7
							 0b01111111,//8
							 0b01101111,//9
							 0b00000000, //turned off
							 0b01000000, //-
							 };

ControllerState controllerState = {0};
ControllerTestProgram testProgram = {0};
TestingParams testingParams = {0};

TaskHandle_t aligninThreadHandle = NULL;
TaskHandle_t testingTreadHandle = NULL;
