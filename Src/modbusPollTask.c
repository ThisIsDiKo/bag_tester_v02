/*
 * modbusPollTask.c
 *
 *  Created on: 28 мая 2020 г.
 *      Author: ADiKo
 */


#include "modbusPollTask.h"

#include "globals.h"
#include "structures.h"

#include "mb.h"
#include "mbport.h"
#include "mbutils.h"


#define REG_STATE_START 1000
#define REG_STATE_NREGS 4

#define REG_TEST_PROGRAM_START 2000
#define REG_TEST_PROGRAM_NREGS 16

//static USHORT   usRegInputStart = REG_INPUT_START;

/* ----------------------- Defines ------------------------------------------*/
#define REG_COILS_START     2000
#define REG_COILS_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
static unsigned char ucRegCoilsBuf[REG_COILS_SIZE / 8];

void xModbusPollTask(void *arguments){
	for(;;){
		( void )eMBPoll(  );
		vTaskDelay(50);
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

//        while( usNRegs > 0 )
//        {
//            *pucRegBuffer++ =
//                ( unsigned char )( controllerState.dataToSend[iRegIndex] >> 8 );
//            *pucRegBuffer++ =
//                ( unsigned char )( controllerState.dataToSend[iRegIndex] & 0xFF );
//            iRegIndex++;
//            usNRegs--;
//        }
    }
    else if( ( usAddress == REG_TEST_PROGRAM_START ) && ( usAddress + usNRegs <= REG_TEST_PROGRAM_START + REG_TEST_PROGRAM_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - REG_TEST_PROGRAM_START );
        usNRegs = usNRegs / 3;
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( testProgram.testPressure[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( testProgram.testPressure[iRegIndex] & 0xFF );
            *pucRegBuffer++ = ( unsigned char )( testProgram.testTime[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( testProgram.testTime[iRegIndex] & 0xFF );
            *pucRegBuffer++ = ( unsigned char )( testProgram.testDiffPressure[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( testProgram.testDiffPressure[iRegIndex] & 0xFF );
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

    if( ( usAddress == REG_TEST_PROGRAM_START ) && ( usAddress + usNRegs <= REG_TEST_PROGRAM_START + REG_TEST_PROGRAM_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - REG_TEST_PROGRAM_START );
        usNRegs = usNRegs / 3;

        switch (eMode){
			case MB_REG_READ:{
		        while( usNRegs > 0 )
		        {
		            *pucRegBuffer++ = ( unsigned char )( testProgram.testPressure[iRegIndex] >> 8 );
		            *pucRegBuffer++ = ( unsigned char )( testProgram.testPressure[iRegIndex] & 0xFF );
		            *pucRegBuffer++ = ( unsigned char )( testProgram.testTime[iRegIndex] >> 8 );
		            *pucRegBuffer++ = ( unsigned char )( testProgram.testTime[iRegIndex] & 0xFF );
		            *pucRegBuffer++ = ( unsigned char )( testProgram.testDiffPressure[iRegIndex] >> 8 );
		            *pucRegBuffer++ = ( unsigned char )( testProgram.testDiffPressure[iRegIndex] & 0xFF );
		            iRegIndex++;
		            usNRegs--;
		        }
		        break;
			}
			case MB_REG_WRITE:{
				while( usNRegs > 0 ){
					//testProgram.testPressure[iRegIndex] = (uint16_t) ((*pucRegBuffer) << 8) + (*(pucRegBuffer + 1));
					testProgram.testPressure[iRegIndex] = (uint16_t) ((*pucRegBuffer++) << 8);
					testProgram.testPressure[iRegIndex] += *pucRegBuffer++;
//					pucRegBuffer++;
//					pucRegBuffer++;
					testProgram.testTime[iRegIndex] = (uint16_t) ((*pucRegBuffer) << 8) + (*(pucRegBuffer + 1));
					pucRegBuffer++;
					pucRegBuffer++;
					testProgram.testDiffPressure[iRegIndex] = (uint16_t) ((*pucRegBuffer) << 8) + (*(pucRegBuffer + 1));
					pucRegBuffer++;
					pucRegBuffer++;
		            iRegIndex++;
		            usNRegs--;
				}

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


