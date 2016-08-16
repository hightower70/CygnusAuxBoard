/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    System timer (1ms) driver                                              */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <Helpers.h>
#include <Timer.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define TIMER_PRESCALER 64
#define TIMER_PERIOD (F_CPU / TIMER_PRESCALER + 500) / 1000

///////////////////////////////////////////////////////////////////////////////
// Module local variables
static volatile uint16_t l_system_time = 0;

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets current timestamp
uint16_t TIMERGetTime(void)
{
	uint16_t retval;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		retval = l_system_time;
	}

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Delay (ms resolution delay)
void TIMERDelay(uint16_t in_delay_ms)
{
	uint16_t start_time = TIMERGetTime();
	uint16_t diff_time;
	
	do
	{
		diff_time = TIMERGetTime() - start_time;
	}	while( diff_time < in_delay_ms );
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets elapsed time since a timestamp (in ms)
uint16_t TIMERGetTimeSince(uint16_t in_start_time_ms)
{
	uint16_t diff_time;

	diff_time = TIMERGetTime() - in_start_time_ms;
	
	return diff_time;
}


#ifdef __AVR_ATmega328P__
/*****************************************************************************/
/* ATMEGA328P Processor specific code                                        */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize system timer
void TIMERInit(void)
{
	TCCR0A = BV(WGM01);
	TCCR0B = BV(CS01) | BV(CS00);
	OCR0A = TIMER_PERIOD;
	TIMSK0 |= BV(OCIE0A);
}

///////////////////////////////////////////////////////////////////////////////
// Interrupt handler
ISR(TIMER0_COMPA_vect)
{
	l_system_time++;
}

#endif

#ifdef __AVR_ATmega8__
/*****************************************************************************/
/* ATMEGA8 Processor specific code                                           */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize system timer
void TIMERInit(void)
{
	// Init timer0
	TCCR0 = BV(CS01) | BV(CS00);
	TCNT0 = TIMER_RELOAD_VALUE;
	TIMSK |= BV(TOIE0);
}

///////////////////////////////////////////////////////////////////////////////
// Interrupt handler
ISR(SIG_OVERFLOW0)
{
	// set 1ms period
	TCNT0 = TIMER_RELOAD_VALUE;

	l_system_time++;
}

#endif