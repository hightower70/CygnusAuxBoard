/*****************************************************************************/
/* Main loop                                                                 */
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
#include <avr/interrupt.h>
#include <Timer.h>
#include <ADC.h>
#include <UART.h>
#include <i2c.h>
#include <UltrasonicDistance.h>
#include <Packets.h>
#include <DigitalSequencer.h>

/*****************************************************************************/
/* Types                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Module global variables                                                   */
/*****************************************************************************/

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// @brief Program main entry 
int main(void)
{
	// init peripheral
	TIMERInit();
	//I2CInit();
	ADCinit();
	//UARTInit();
	//USDInit();
	DSInit();

	DSSelectSequence(0);

	// enable interrupt
	sei();

	while(1)
	{
		//USDProcess();
		DSProcess();
	}
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Ultrasonic distance measurement process
void USDProcess(void)
{
	FNPDistance distance_packet;
	USDMeasuredData measured_data;

	// send packet
	if(USDGetMeasuredData(&measured_data))
	{
		distance_packet.Type = PT_DISTANCE;
		distance_packet.Distance = measured_data.Distance;
		distance_packet.Temperature = measured_data.Temperature;

		UARTSendSlipBlock((uint8_t*)&distance_packet, sizeof(distance_packet));
	}
}

