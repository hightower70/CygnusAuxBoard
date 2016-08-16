/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    SPI bus driver                                                         */
/*****************************************************************************/

 //////////////////////////////////////////////////////////////////////////////
 // Includes
 #include <avr/io.h>
 #include <stdint.h>
 #include <Helpers.h>
 
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
