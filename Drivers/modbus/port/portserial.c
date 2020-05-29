/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include "stm32f1xx_hal.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "globals.h"

/* ----------------------- static functions ---------------------------------*/


void prvvUARTTxReadyISR( void );
void prvvUARTRxISR( void );

extern UART_HandleTypeDef huart1;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	if (xRxEnable){
		MODBUS_MODE_RX;
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	}
	else{
		__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
	}

	if (xTxEnable){
		MODBUS_MODE_TX;
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
	}
	else{
		__HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
	}
}

void vMBPortSerialEnableTC( BOOL xTcEnable){
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TC);
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
//	switch (ucPORT)
//	  {
//		case 0:
//	          huart_m.Instance = USART1;
//	          break;
//	 	case 1:
//	          huart_m.Instance = USART2;
//	          break;
//		case 2:
//	          huart_m.Instance = USART3;
//	          break;
//	        default:
//	          return FALSE;
//	  }
//	  huart_m.Init.BaudRate = ulBaudRate;
//	  switch (ucDataBits)
//	  {
//	        case 8:
//	                huart_m.Init.WordLength = UART_WORDLENGTH_8B;
//	                break;
//	        default:
//	                return FALSE;
//	  }
//	  switch (eParity)
//	  {
//	    case MB_PAR_NONE:
//	            huart_m.Init.Parity = UART_PARITY_NONE;
//	            break;
//	    case MB_PAR_EVEN:
//	            huart_m.Init.Parity = UART_PARITY_EVEN;
//	            break;
//	    case MB_PAR_ODD:
//	            huart_m.Init.Parity = UART_PARITY_ODD;
//	            break;
//	    default:
//	            return FALSE;
//	  }
//	  huart_m.Init.StopBits = UART_STOPBITS_1;
//	  huart_m.Init.Mode = UART_MODE_TX_RX;
//	  huart_m.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	  huart_m.Init.OverSampling = UART_OVERSAMPLING_16;
//	  return (HAL_OK == USART_Init(&huart_m));
	return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	huart1.Instance -> DR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    if(huart1.Init.Parity == UART_PARITY_NONE)
    {
        *pucByte = (uint8_t)(huart1.Instance->DR & (uint8_t)0x00FF);
    }
    else

    {
        *pucByte = (uint8_t)(huart1.Instance->DR & (uint8_t)0x007F);
    }
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static unsigned int uiCnt = 0;
void prvvUARTTxReadyISR( void )
{
//	if (uiCnt++ < 10){
//		(void) xMBPortSerialPutByte('a');
//	}else{
//		vMBPortSerialEnable(FALSE, FALSE);
//	}
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR( void )
{
//	CHAR cByte;
//	(void) xMBPortSerialGetByte(&cByte);
    pxMBFrameCBByteReceived(  );
}

