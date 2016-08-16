/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    I2C driver                                                             */
/*****************************************************************************/


#ifndef __I2C_h
#define __I2C_h

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <stdint.h>
#include <Helpers.h>

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
void I2CInit(void);
void I2CStartWriteBlock(uint8_t in_address, uint8_t* in_buffer, uint8_t in_buffer_length);
void I2CStartReadBlock(uint8_t in_address, uint8_t* in_read_buffer, uint8_t in_read_buffer_length);
bool_t I2CWaitForReady();



#endif