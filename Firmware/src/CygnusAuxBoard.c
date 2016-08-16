/*****************************************************************************/
/*                                                                           */
/*    Cygnus Aux Board Firmware                                              */
/*                                                                           */
/*    Copyright (C) 2016 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    Main loop                                                              */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Timer.h>
#include <ADC.h>
#include <UART.h>
#include <i2c.h>
#include <UltrasonicDistance.h>
#include <Packets.h>
#include <DigitalSequencer.h>

///////////////////////////////////////////////////////////////////////////////
// Types

///////////////////////////////////////////////////////////////////////////////
// Module global variables

///////////////////////////////////////////////////////////////////////////////
// Function prototypes

///////////////////////////////////////////////////////////////////////////////
// Main function
int main(void)
{
	// init peripheral
	TIMERInit();
	//I2CInit();
	//ADCinit();
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
// Ultrasonic distance measurement process
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

