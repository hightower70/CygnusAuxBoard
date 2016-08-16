/*****************************************************************************/
/*                                                                           */
/*    Cygnus Aux Board Firmware                                              */
/*                                                                           */
/*    Copyright (C) 2016 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    Digital output sequencer                                               */
/*****************************************************************************/

#ifndef __DigitalSequencer_h
#define __DigitalSequencer_h

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/eeprom.h>
#include <stdint.h>
#include <Helpers.h>


///////////////////////////////////////////////////////////////////////////////
// Macros
#define DS_MS_TO_TICK(x) (x / 10)
#define DS_TICK_TO_MS(x) (x * 10)
#define DS_END 0

///////////////////////////////////////////////////////////////////////////////
// Types
typedef struct __attribute__ ((packed))
{
	uint8_t Delay;
	uint8_t Data;
} DSTableEntry;


extern DSTableEntry EEMEM g_digital_sequence_table[];

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
void DSInit(void);
void DSProcess(void);
void DSSelectSequence(uint8_t in_sequence_index);


#endif