/*****************************************************************************/
/* UART Driver using SLIP protocol                                           */
/*                                                                           */
/* Copyright (C) 2016 Laszlo Arvai                                           */
/* All rights reserved.                                                      */
/*                                                                           */
/* This program is free software: you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation, either version 3 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*****************************************************************************/

#ifndef __UART_h
#define __UART_h

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <stdint.h>
#include <Helpers.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
// SLIP constants
#define UART_SLIP_END 0xc0
#define UART_SLIP_ESC 0xdb
#define UART_SLIP_ESC_END 0xdc
#define UART_SLIP_ESC_ESC 0xdd

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
void UARTInit(void);
void UARTSendByte(uint8_t in_byte);
void UARTSendSlipByte(uint8_t in_byte);
void UARTSendSlipBlock(uint8_t* in_block, uint8_t in_block_length);
void UARTWaitForTransmitEmpty(void);

bool_t UARTIsReceivedCharacterAvailable(void);
uint8_t UARTGetReceivedCharacter(void);

#endif 