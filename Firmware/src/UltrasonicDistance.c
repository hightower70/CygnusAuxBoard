/*****************************************************************************/
/*                                                                           */
/*    Cygnus Aux Board Firmware                                              */
/*                                                                           */
/*    Copyright (C) 2016 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    Ultrasonic distance sensor driver                                      */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Helpers.h>
#include <UART.h>
#include <ADC.h>
#include <UltrasonicDistance.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define REPAT_RATE 10 // repeat measurements at 10Hz
#define TIMER_PRESCALER 64
#define TIMER_TOP (F_CPU/TIMER_PRESCALER/REPEAT_RATE)
#define PULSE_LENGTH 3

///////////////////////////////////////////////////////////////////////////////
// Module global variables
static uint16_t l_start_time;
static uint16_t l_delta_time;
static bool_t l_measurement_ready;
static uint8_t l_current_channel = 0;

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize ultrasonic distance sensor
void USDInit(void)
{
	// init port pins
	TRIG_PORT &= ~BV(TRIG_PIN);

	TRIG_DDR |= BV(TRIG_PIN);

	USEL0_PORT |= BV(USEL0_PIN);
	USEL1_PORT |= BV(USEL1_PIN);

	USEL0_DDR |= BV(USEL0_PIN);
	USEL1_DDR |= BV(USEL1_PIN);

	// setup timer
	OCR1A = TIMER_TOP;
	OCR1B = PULSE_LENGTH;

	TCCR1A = 0;
	TCCR1B = BV(WGM12) | BV(CS11) | BV(CS10) | BV(ICES1);

#ifdef __AVR_ATmega8__
	TIMSK |= BV(TICIE1) | BV(OCIE1A) | BV(OCIE1B);
#endif

#ifdef __AVR_ATmega328P__
	TIMSK1 |= BV(ICIE1) | BV(OCIE1A) | BV(OCIE1B);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Returns measurement status
/// @return True is new measurement data is available
bool_t USDIsDataAvailable(void)
{
	return l_measurement_ready;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets distance measured data
/// @param out_data Distance measured data to fill out
/// @return True is new measurement is done since the last call of this function. If no data available the struct is not updated.
bool_t USDGetMeasuredData(USDMeasuredData* out_data)
{
	bool_t retval = l_measurement_ready;

	if(retval)
	{
		out_data->Temperature = g_temperature;

		// speed of sound calculated by: c = 331.3 + 0.606 * T, distance calculated by = delta time / speed of sound / 2
		out_data->Distance = (uint16_t)((331300 + 606 * (uint32_t)g_temperature / 10) * (TIMER_PRESCALER / 2) / 1000 * l_delta_time / (F_CPU / 1000));

		l_measurement_ready = boolFalse;
	}

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Input capture interrupt
ISR(TIMER1_CAPT_vect)
{
	uint16_t time = ICR1;

	if((TCCR1B & BV(ICES1)) != 0)
	{
		// rising edge detected
		l_start_time = time; 

		// falling edge
		TCCR1B &= ~BV(ICES1);
	}
	else
	{
		// falling edge detected
		l_delta_time = time - l_start_time;
		l_measurement_ready = boolTrue;

		// prepare for rising edge
		TCCR1B |= BV(ICES1);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Compare A interrupt, TRIG start
ISR(TIMER1_COMPA_vect)
{
	// set USEL0 pin
	if((l_current_channel & 0x01) == 0)
		USEL0_PORT &= ~BV(USEL0_PIN);
	else
		USEL0_PORT |= BV(USEL0_PIN);

	// set USEL1 pin
	if((l_current_channel & 0x02) == 0)
	USEL1_PORT &= ~BV(USEL1_PIN);
	else
	USEL1_PORT |= BV(USEL1_PIN);

	// start trigger
	TRIG_PORT |= BV(TRIG_PIN);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Compare B interrupt, TRIG end
ISR(TIMER1_COMPB_vect)
{
	// end trigger
	TRIG_PORT &= ~BV(TRIG_PIN);
}