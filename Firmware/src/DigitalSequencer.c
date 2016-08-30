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

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <avr/io.h>
#include <util/atomic.h>
#include <Timer.h>
#include <DigitalSequencer.h>
#include "IOConfig.h"

/*****************************************************************************/
/* Module global variables                                                   */
/*****************************************************************************/
static uint16_t l_timestamp;
static uint16_t l_current_delay = 0;
static uint16_t l_sequence_start_address;
static uint16_t l_current_sequence_address;

/*****************************************************************************/
/* Module local functions                                                    */
/*****************************************************************************/
static uint16_t DSLoadNextStep(void);

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize digital sequencer 
void DSInit(void)
{
	l_current_delay = 0;

	DIO1_PORT &= ~BV(DIO1_PIN);
	DIO2_PORT &= ~BV(DIO2_PIN);
	DIO3_PORT &= ~BV(DIO3_PIN);

	DIO1_DDR |= BV(DIO1_PIN);
	DIO2_DDR |= BV(DIO2_PIN);
	DIO3_DDR |= BV(DIO3_PIN);

}

///////////////////////////////////////////////////////////////////////////////
/// @brief Select and start the given sequence
/// @param in_sequence_idnex Sequence to start
void DSSelectSequence(uint8_t in_sequence_index)
{
	uint16_t prev_delay;
	DSTableEntry sequence_step;
	uint16_t sequence_address;

	sequence_address = (uint16_t)(&g_digital_sequence_table[0]);
	prev_delay = 1; // something non zero
	while(in_sequence_index > 0)
	{
		eeprom_read_block(&sequence_step, &g_digital_sequence_table[sequence_address], sizeof(DSTableEntry));

		if(sequence_step.Delay == DS_END)
		{
			in_sequence_index--;

			// End of the list
			if(prev_delay == DS_END)
			{
				l_current_delay = 0;
				break;
			}
		}

		sequence_address++;
		prev_delay = sequence_step.Delay;
	}

	// set sequence
	l_sequence_start_address = sequence_address;
	l_current_sequence_address = sequence_address;
	l_current_delay = DSLoadNextStep();
	l_timestamp = TIMERGetTime();
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Digital sequencer process
void DSProcess(void)
{
	// check if sequencer is active
	if(l_current_delay == 0)
		return;

	// check if delay is expired
	if(TIMERGetTimeSince(l_timestamp) < l_current_delay)
		return;

	// switch to next step
	l_current_delay = DSLoadNextStep();
	l_timestamp = TIMERGetTime();
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Loads next sequencer step from the EEPROM and sends port value to the appropriate port bits
/// @return Delay time of this step
static uint16_t DSLoadNextStep(void)
{
	DSTableEntry sequence_step;

	eeprom_read_block(&sequence_step, &g_digital_sequence_table[l_current_sequence_address], sizeof(DSTableEntry));

	// check for sequence end
	if(sequence_step.Delay == DS_END)
	{
		l_current_sequence_address = l_sequence_start_address;
		eeprom_read_block(&sequence_step, &g_digital_sequence_table[l_current_sequence_address], sizeof(DSTableEntry));
	}

	// next step in the sequence
	l_current_sequence_address++;

	// empty sequence
	if(sequence_step.Delay == DS_END)
		return DS_END;

	// update digital output
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// bit0
		if((sequence_step.Data & 0x01) == 0)
			DIO1_PORT &= ~BV(DIO1_PIN);
		else
			DIO1_PORT |= BV(DIO1_PIN);

		// bit1
		if((sequence_step.Data & 0x02) == 0)
			DIO2_PORT &= ~BV(DIO2_PIN);
		else
			DIO2_PORT |= BV(DIO2_PIN);

		// bit2
		if((sequence_step.Data & 0x04) == 0)
			DIO3_PORT &= ~BV(DIO3_PIN);
		else
			DIO3_PORT |= BV(DIO3_PIN);
	}

	return DS_TICK_TO_MS(sequence_step.Delay);
}

