/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    UART Driver using SLIP protocol                                        */
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