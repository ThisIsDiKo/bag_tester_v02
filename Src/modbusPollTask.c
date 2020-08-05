/*
 * modbusPollTask.c
 *
 *  Created on: 28 мая 2020 г.
 *      Author: ADiKo
 */


#include "modbusPollTask.h"

#include "globals.h"
#include "structures.h"

#include "flashFunctions.h"

#include "mb.h"
#include "mbport.h"
#include "mbutils.h"


#define REG_STATE_START 1000
#define REG_STATE_NREGS 3

#define REG_TEST_PROGRAM_START 2000
#define REG_TEST_PROGRAM_NREGS 18

#define TIME_TEST_PROGRAM_START 3000
#define TIME_TEST_PROGRAM_NREGS 3

//static USHORT   usRegInputStart = REG_INPUT_START;

/* ----------------------- Defines ------------------------------------------*/
#define REG_COILS_START     2000
#define REG_COILS_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
static unsigned char ucRegCoilsBuf[REG_COILS_SIZE / 8];

uint8_t needToWriteFlash = 0;

uint16_t modbusArray[REG_TEST_PROGRAM_NREGS] = {0};

void xModbusPollTask(void *arguments){
	for(;;){
		( void )eMBPoll(  );
		vTaskDelay(50);
		if (needToWriteFlash){
			controllerState.state = IDLE;
			mWrite_flash();
			needToWriteFlash = 0;
		}
	}
	vTaskDelete(NULL);
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int8_t         iRegIndex;

    if( ( usAddress == REG_STATE_START ) && ( usAddress + usNRegs <= REG_STATE_START + REG_STATE_NREGS))
    {
        iRegIndex = (int8_t)( usAddress - REG_STATE_START );
        controllerState.dataToSend[1] = controllerState.currentStep;
        controllerState.dataToSend[2] = (uint16_t)controllerState.state;
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( controllerState.dataToSend[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( controllerState.dataToSend[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int8_t         iRegIndex;

    modbusArray[0] = testProgram.testPressure[0];
    modbusArray[1] = testProgram.testTime[0];
    modbusArray[2] = testProgram.testDiffPressure[0];

    modbusArray[3] = testProgram.testPressure[1];
    modbusArray[4] = testProgram.testTime[1];
    modbusArray[5] = testProgram.testDiffPressure[1];

    modbusArray[6] = testProgram.testPressure[2];
    modbusArray[7] = testProgram.testTime[2];
    modbusArray[8] = testProgram.testDiffPressure[2];

    modbusArray[9] = testProgram.testPressure[3];
    modbusArray[10] = testProgram.testTime[3];
    modbusArray[11] = testProgram.testDiffPressure[3];

    modbusArray[12] = testProgram.testPressure[4];
    modbusArray[13] = testProgram.testTime[4];
    modbusArray[14] = testProgram.testDiffPressure[4];

    modbusArray[15] = testProgram.delayAfterAligningSeconds;
    modbusArray[16] = testProgram.checkingDelaySeconds;
    modbusArray[17] = testProgram.maxNumberOfBadPressure;

    if( ( usAddress == REG_TEST_PROGRAM_START ) && ( usAddress + usNRegs <= REG_TEST_PROGRAM_START + REG_TEST_PROGRAM_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - REG_TEST_PROGRAM_START );

        switch (eMode){
			case MB_REG_READ:{
		        while( usNRegs > 0 )
		        {
		            *pucRegBuffer++ = ( unsigned char )( modbusArray[iRegIndex] >> 8 );
		            *pucRegBuffer++ = ( unsigned char )( modbusArray[iRegIndex] & 0xFF );
		            iRegIndex++;
		            usNRegs--;
		        }
		        break;
			}
			case MB_REG_WRITE:{
				if (usNRegs > 17){
					//testProgram.testPressure[iRegIndex] = (uint16_t) ((*pucRegBuffer) << 8) + (*(pucRegBuffer + 1));
					testProgram.testPressure[0] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testPressure[0] += *pucRegBuffer++;
					testProgram.testTime[0] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testTime[0] += *pucRegBuffer++;
					testProgram.testDiffPressure[0] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testDiffPressure[0] += *pucRegBuffer++;

					testProgram.testPressure[1] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testPressure[1] += *pucRegBuffer++;
					testProgram.testTime[1] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testTime[1] += *pucRegBuffer++;
					testProgram.testDiffPressure[1] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testDiffPressure[1] += *pucRegBuffer++;

					testProgram.testPressure[2] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testPressure[2] += *pucRegBuffer++;
					testProgram.testTime[2] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testTime[2] += *pucRegBuffer++;
					testProgram.testDiffPressure[2] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testDiffPressure[2] += *pucRegBuffer++;

					testProgram.testPressure[3] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testPressure[3] += *pucRegBuffer++;
					testProgram.testTime[3] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testTime[3] += *pucRegBuffer++;
					testProgram.testDiffPressure[3] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testDiffPressure[3] += *pucRegBuffer++;

					testProgram.testPressure[4] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testPressure[4] += *pucRegBuffer++;
					testProgram.testTime[4] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testTime[4] += *pucRegBuffer++;
					testProgram.testDiffPressure[4] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testDiffPressure[4] += *pucRegBuffer++;

					testProgram.delayAfterAligningSeconds = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.delayAfterAligningSeconds += *pucRegBuffer++;
					testProgram.checkingDelaySeconds = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.checkingDelaySeconds += *pucRegBuffer++;
					testProgram.maxNumberOfBadPressure = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.maxNumberOfBadPressure += *pucRegBuffer++;

		            iRegIndex++;
		            usNRegs--;
				}
				needToWriteFlash = 1;
				break;
			}
        }
    }
    else{
    	eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNCoils = ( short )usNCoils;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
                /* Read current values and pass to protocol stack. */
            case MB_REG_READ:
                while( iNCoils > 0 )
                {
                    *pucRegBuffer++ =
                        xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                        ( unsigned char )( iNCoils >
                                                           8 ? 8 :
                                                           iNCoils ) );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;

                /* Update current register values. */
            case MB_REG_WRITE:
                while( iNCoils > 0 )
                {
                    xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                                    ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                                    *pucRegBuffer++ );
                    iNCoils -= 8;
                }

                if (ucRegCoilsBuf[0] & 0x01) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
                else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

                break;
        }

    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}


