/*****************************************************************************/
/* SPI bus driver                                                            */
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
#include <stdint.h>
#include <Helpers.h>

/*****************************************************************************/
/* Function implementation                                                   */
/*****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
/// @brief Initializes SPI bus
void SPIInit(void)
{
	// Setup Port B outputs
	DDRB |= BV(PB3) | BV(PB5);

	SPCR = BV(CPOL) | BV(CPHA) | BV(SPE)| BV(MSTR) | BV(SPR0);  
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Send and receives byte over SPI bus
/// @param Data to send
/// @return Data read from the bus
uint8_t SPIWriteAndReadData(uint8_t in_data)
{
	SPDR = in_data;

	while(!(SPSR & BV(SPIF)));  //SPSR = SPI Status Register, SPIF = tx complete

	return SPDR;
}
