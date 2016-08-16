/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    ADC driver                                                             */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <UART.h>
#include <IOConfig.h>
#include <Helpers.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define CHANNEL_COUNT 2
#define TEMPERATURE_AVG_COUNT 512
#define ADC_MAX_VALUE 1024
#define LM75_CALIBRATION_VALUE 2560

///////////////////////////////////////////////////////////////////////////////
// Module global variables
static uint8_t l_current_channel = 0;
static uint16_t l_current_temperature;
static uint32_t l_temperature_avg = 0;
static uint16_t l_temperature_avg_count;
uint16_t g_temperature;

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize ADC driver
void ADCinit(void)
{
	// Init A/D converter
	ADMUX = 0;
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	
	
	// init variables
	l_temperature_avg_count = 0;
	l_temperature_avg = 0;

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
			l_current_temperature = adc;

			mux |= 1; // Select ADC channel 1
			break;
		
		case 1:
			l_current_temperature = l_current_temperature - adc;
			l_temperature_avg += l_current_temperature;
			l_temperature_avg_count++;
			if(l_temperature_avg_count >= TEMPERATURE_AVG_COUNT)
			{
				g_temperature = (int16_t)(((int32_t)l_temperature_avg * LM75_CALIBRATION_VALUE / TEMPERATURE_AVG_COUNT / ADC_MAX_VALUE));
				l_temperature_avg = 0;
				l_temperature_avg_count = 0;
			}

			mux |= 0; // Select ADC channel 0
			break;
	}

	// next channel	
	l_current_channel++;
	if(l_current_channel >= CHANNEL_COUNT)
		l_current_channel = 0;
	
	// update mux
	ADMUX = mux;
	
	// restart conversion
	ADCSRA |= _BV(ADSC);
}
