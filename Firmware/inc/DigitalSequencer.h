/*****************************************************************************/
/* Digital output sequencer                                                  */
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

#ifndef __DigitalSequencer_h
#define __DigitalSequencer_h

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <avr/eeprom.h>
#include <stdint.h>
#include <Helpers.h>

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/
#define DS_MS_TO_TICK(x) (x / 10)
#define DS_TICK_TO_MS(x) (x * 10)
#define DS_END 0

/*****************************************************************************/
/* Types                                                                     */
/*****************************************************************************/
typedef struct __attribute__ ((packed))
{
	uint8_t Delay;
	uint8_t Data;
} DSTableEntry;

/*****************************************************************************/
/* Global variables                                                          */
/*****************************************************************************/
extern DSTableEntry EEMEM g_digital_sequence_table[];

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void DSInit(void);
void DSProcess(void);
void DSSelectSequence(uint8_t in_sequence_index);


#endif