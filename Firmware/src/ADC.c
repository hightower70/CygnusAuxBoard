/*****************************************************************************/
/* ADC driver                                                                */
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

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <UART.h>
#include <IOConfig.h>
#include <Helpers.h>
#include <ADC.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define ADC_CHANNEL_COUNT 2
#define ADC_TEMPERATURE_AVG_COUNT 512
#define ADC_AIN1_AVG_COUNT 128
#define ADC_MAX_VALUE 1024

///////////////////////////////////////////////////////////////////////////////
// Module global variables
static uint8_t l_current_channel = 0;
static uint16_t l_current_temperature;

static uint32_t l_temperature_avg = 0;
static uint16_t l_temperature_avg_count;
uint16_t g_temperature_raw;
uint16_t g_temperature;

static uint32_t l_ain1_avg = 0;
static uint16_t l_ain1_avg_count;
uint16_t g_ain1_raw;

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize ADC driver
void ADCinit(void)
{
	// Init A/D converter
	ADMUX = _BV(REFS0) | ADC_TEMP;
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	
	
	// init variables
	l_temperature_avg_count = 0;
	l_temperature_avg = 0;
	l_current_channel = 0;

	// start conversion
	ADCSRA |= _BV(ADSC);
}


///////////////////////////////////////////////////////////////////////////////
/// @brief Interrupt handler
ISR(ADC_vect)
{
	uint8_t mux = ADMUX;
	uint16_t adc = ADC;
	
	// select input
	mux &= 0xf0;
	
	switch(l_current_channel)
	{
		case 0:
			l_temperature_avg  += adc;
			l_temperature_avg_count++;
			if(l_temperature_avg_count >= ADC_TEMPERATURE_AVG_COUNT)
			{
				g_temperature_raw = l_temperature_avg / ADC_TEMPERATURE_AVG_COUNT;
				l_temperature_avg = 0;
				l_temperature_avg_count = 0;
			}

			// next channel
			mux |= ADC_AIN1;
			break;
		
		case 1:
			l_ain1_avg += adc;
			l_ain1_avg_count++;
			if(l_ain1_avg_count >= ADC_AIN1_AVG_COUNT)
			{
				g_ain1_raw = l_ain1_avg / ADC_AIN1_AVG_COUNT;
				l_ain1_avg = 0;
				l_ain1_avg_count = 0;
			}

			// next channel
			mux |= ADC_TEMP;
			break;
	}

	// next channel	
	l_current_channel++;
	if(l_current_channel >= ADC_CHANNEL_COUNT)
		l_current_channel = 0;
	
	// update mux
	ADMUX = mux;
	
	// restart conversion
	ADCSRA |= _BV(ADSC);
}
