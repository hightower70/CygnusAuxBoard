/*****************************************************************************/
/*                                                                           */
/*    Cygnus Aux Board Firmware                                              */
/*                                                                           */
/*    Copyright (C) 2016 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    LED flashing sequence data                                             */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/eeprom.h>
#include <DigitalSequencer.h>

DSTableEntry EEMEM g_digital_sequence_table[] =
{
	{DS_MS_TO_TICK(120), 1 },
	{DS_MS_TO_TICK(120), 0 },

	{DS_MS_TO_TICK(120), 2 },
	{DS_MS_TO_TICK(120), 0 },

	{DS_MS_TO_TICK(520), 0 },

	{DS_END, 0},

	{DS_END, 0}
};

