/*
 * flashFunctions.c
 *
 *  Created on: 3 ���. 2020 �.
 *      Author: ADiKo
 */


/*
 * flashFunctions.c
 *
 *  Created on: 16 ����. 2019 �.
 *      Author: ADiKo
 */

#include "globals.h"
#include "structures.h"
#include "flashFunctions.h"

#define CONTROLLER_LENGTH_WORDS		(sizeof(testProgram)	/ 4)

void mWrite_flash(void){
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t pageError;
	uint16_t i;

	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = SETTINGS_FLASH_PAGE_ADDR;
	EraseInitStruct.NbPages = 1;

	if(HAL_FLASHEx_Erase(&EraseInitStruct, &pageError) != HAL_OK){
		//print_debug("Erase Flash Error\n");
	}

	uint32_t *source_addr = (void *)&testProgram;
	uint32_t *dest_addr = (uint32_t *) SETTINGS_FLASH_PAGE_ADDR;
	for (i=0; i<CONTROLLER_LENGTH_WORDS; i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)dest_addr, *source_addr);
//		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)dest_addr, *source_addr)!= HAL_OK){
//			//print_debug("Write Flash Error\n");
//			asm()
//		}
		source_addr++;
		dest_addr++;
	}

	HAL_FLASH_Lock();
	//print_debug("Flash ok\n");
}

void mRead_flash(void){
	//print_debug("reading flash\n");

	uint32_t *source_addr = (uint32_t *)SETTINGS_FLASH_PAGE_ADDR;
    uint32_t *dest_addr = (void*)&testProgram;

    for (uint16_t i=0; i<CONTROLLER_LENGTH_WORDS; i++) {
        *dest_addr = *(__IO uint32_t*)source_addr;
        source_addr++;
        dest_addr++;
    }

   // print_debug("Flash read\n");
}
