/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    I2C driver                                                             */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <IOConfig.h>
#include <Helpers.h> 
#include <I2C.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define TWCR_DEFAULT (BV(TWEN) | BV(TWIE))

#define ST_BUSY BV(7)
#define ST_ERROR BV(0)

///////////////////////////////////////////////////////////////////////////////
// Types

///////////////////////////////////////////////////////////////////////////////
// Module global variables
static volatile uint8_t l_status;
static uint8_t l_address;
static uint8_t* l_write_buffer;
static uint8_t l_write_buffer_length;
static uint8_t l_write_buffer_pos;
static uint8_t* l_read_buffer;
static uint8_t l_read_buffer_length;
static uint8_t l_read_buffer_pos;

///////////////////////////////////////////////////////////////////////////////
/// @brief Initializes I2C driver
void I2CInit(void)
{
	// set baud register
  TWBR = ((F_CPU / I2C_FREQ) - 16) / (2 * 1);

	// internal pull-up
  PORTC |= BV(PC5) | BV(PC4);

  // enable interrupts and module
  TWCR = TWCR_DEFAULT;

  // Disable slave mode
  TWAR = 0;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Starts writing block of bytes
/// @param in_address I2C address [0h..7fh]
/// @param in_buffer Data buffer address
/// @param in_buffer_length Number of bytes to write
void I2CStartWriteBlock(uint8_t in_address, uint8_t* in_buffer, uint8_t in_buffer_length)
{
	// store communication info
	l_address = (in_address << 1) | TW_WRITE;
	l_write_buffer = in_buffer;
	l_write_buffer_length = in_buffer_length;
	l_write_buffer_pos = 0;
	l_read_buffer_length = 0;
	l_read_buffer_pos = 0;

	// start communication
	l_status = ST_BUSY;
	TWCR = TWCR_DEFAULT | BV(TWINT) | BV(TWSTA);
}

void I2CStartReadBlock(uint8_t in_address, uint8_t* in_read_buffer, uint8_t in_read_buffer_length)
{
	// store communication info
	l_address = (in_address << 1) | TW_READ;
	l_write_buffer = NULL;
	l_write_buffer_length = 0;
	l_write_buffer_pos = 0;
	l_read_buffer = in_read_buffer;
	l_read_buffer_length = in_read_buffer_length;
	l_read_buffer_pos = 0;

	// start communication
	l_status = ST_BUSY;
	TWCR = TWCR_DEFAULT | BV(TWINT) | BV(TWSTA);
}

bool_t I2CWaitForReady()
{
	while((l_status & ST_BUSY) != 0);

	return (l_status & ST_ERROR) == 0;
}

/*****************************************************************************/
/* Interrupt handler                                                         */
/*****************************************************************************/
ISR(TWI_vect, ISR_BLOCK)
{
	uint8_t i2c_hw_status = TW_STATUS;

	if ((l_address & BV(0)) == TW_READ) 
	{
    // Master Transmitter mode
    switch(i2c_hw_status)
    {
	    // A START condition has been transmitted
	    case TW_START:
		    TWDR = l_address | TW_READ;
			  TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;

			// SLA+R has been transmitted; ACK has been received.
			case TW_MR_SLA_ACK:
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWEA);
				break;

			// Data byte has been received
			case TW_MR_DATA_ACK:
				l_read_buffer[l_read_buffer_pos++] = TWDR;
				if (l_read_buffer_pos < l_read_buffer_length-1)
				{
					// receive next byte
					TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWEA);
				}
				else
				{
					// transmit NACK
					TWCR = TWCR_DEFAULT | _BV(TWINT);
				}
				break;

			// send stop condition
			case TW_MR_DATA_NACK:
				l_read_buffer[l_read_buffer_pos] = TWDR;
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				l_status &= ~ST_BUSY;
				break;
		}
	}
	else 
	{
    // Master Transmitter mode
    switch(i2c_hw_status) 
		{
			// A START condition has been transmitted
			case TW_START:
	      TWDR = l_address & 0xf7;
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;

	    // A repeated START condition has been transmitted
			case TW_REP_START:
		    TWDR = l_address & 0x01;
		    TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;

			// SLA+W has been transmitted; ACK has been received.
			case TW_MT_SLA_ACK:
				TWDR = l_write_buffer[l_write_buffer_pos++];
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;

			// Data byte has been transmitted; ACK has been received
			case TW_MT_DATA_ACK:
				if (l_write_buffer_pos < l_write_buffer_length)
				{
					// transmit next byte
					TWDR = l_write_buffer[l_write_buffer_pos++];
					TWCR = TWCR_DEFAULT | _BV(TWINT);
				}
				else
				{
					// transmit stop condition
					TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
					l_status &= ~ST_BUSY;
				}
				break;

			// Arbitration lost in SLA+W or data bytes
			case TW_MT_ARB_LOST:
				// A START condition will be transmitted when the bus becomes free
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTA);
				break;

			// SLA+W has been transmitted; NOT ACK has been received
			case TW_MT_SLA_NACK:
	      l_status |= ST_ERROR;
				// transmit stop condition
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				break;

			// Data byte has been transmitted; NOT ACK has been received
			case TW_MT_DATA_NACK:
				if(l_write_buffer_pos < l_write_buffer_length) 
				{
					// There were more bytes left to transmit!
					l_status |= ST_ERROR;
	      }

				// transmit stop condition
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				break;
		}
	}
}