/*****************************************************************************/
/*                                                                           */
/*    Cygnus Aux Board Firmware                                              */
/*                                                                           */
/*    Copyright (C) 2016 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    UART Driver using SLIP protocol                                        */
/*****************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <UART.h>
#include <IOConfig.h>
#include <Helpers.h>

///////////////////////////////////////////////////////////////////////////////
// Constants & Macros
#define UART_BAUD_SELECT       (F_CPU/(UART_BAUD_RATE*16l)-1)

	 
///////////////////////////////////////////////////////////////////////////////
// Module local variables
uint8_t g_rx_buffer[UART_RX_BUFFER_LENGTH];
uint8_t g_tx_buffer[UART_TX_BUFFER_LENGTH];

static volatile uint8_t l_tx_push_index = 0;			// transmission circular buffer
static volatile uint8_t l_tx_pop_index = 0;

static volatile uint8_t l_rx_push_index = 0;			// receiver circular buffer
static volatile uint8_t l_rx_pop_index = 0;

///////////////////////////////////////////////////////////////////////////////
// Local function prototypes
static void UARTStartTransmission(void);
static void UARTStoreReceivedCharacter(uint8_t in_byte);

/*****************************************************************************/
/* Transmitter section                                                       */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// @brief Send block of data using SLIP encoding
/// @param in_block Pointer to the data block
/// @param in_block_length Length of the block in bytes
void UARTSendSlipBlock( uint8_t* in_block, uint8_t in_block_length)
{
	uint8_t checksum = 0;
	uint8_t data;
	
	// block start
	UARTSendByte(UART_SLIP_END);
	
	// block length
	UARTSendByte(in_block_length);
	checksum = in_block_length;
	
	// send block
	while(in_block_length > 0)
	{
		data = *in_block;
		UARTSendSlipByte(data);
		checksum += data;
		
		in_block++;
		in_block_length--;
	}
	
	// send checksum
	UARTSendSlipByte(checksum);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Send on byte using SLIP encoding
/// @param in_byte Data to send
void UARTSendSlipByte(uint8_t in_byte)
{
	// check to see if is a special character
	switch (in_byte)
	{
		case UART_SLIP_END:
			UARTSendByte(UART_SLIP_ESC);			// escape special character
			UARTSendByte(UART_SLIP_ESC_END);
			break;

		case UART_SLIP_ESC:
			UARTSendByte(UART_SLIP_ESC);			// escape special character
			UARTSendByte(UART_SLIP_ESC_ESC);
			break;

		// send raw character
		default:
			UARTSendByte(in_byte);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Wait for transmitter empty
void UARTWaitForTransmitEmpty(void)
{
	// wait for UART ready
	while( l_tx_pop_index != l_tx_push_index );
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Store byte in the transmission buffer
/// @param in_byte 
void UARTSendByte(uint8_t in_byte)
{
	// buffer pointer
	uint8_t new_push_index = l_tx_push_index + 1;

	if( new_push_index >= UART_TX_BUFFER_LENGTH )
		new_push_index = 0;

	// store dosCharacter
	g_tx_buffer[l_tx_push_index] = in_byte;

	// wait if buffer is full
	while(new_push_index == l_tx_pop_index);

	// store new index
	l_tx_push_index = new_push_index;

	// initiate transmission
	UARTStartTransmission();
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Checks if received character is available
/// @return True if character pending
bool_t UARTIsReceivedCharacterAvailable(void)
{
	return ( l_rx_pop_index != l_rx_push_index );
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets next received character from the queue
/// @return Received character, 0 if no character pending
uint8_t UARTGetReceivedCharacter(void)
{
	uint8_t retval = 0;

	if( l_rx_pop_index != l_rx_push_index )
	{
		// Send next dosCharacter in string
		retval = g_rx_buffer[l_rx_pop_index];

		// step to the next dosCharacter
		l_rx_pop_index++;
		if( l_rx_pop_index >= UART_RX_BUFFER_LENGTH )
			l_rx_pop_index = 0;
	}

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Stores received character
/// @param in_byte Data byte to store
static void UARTStoreReceivedCharacter(uint8_t in_byte)
{
	// buffer pointer
	uint8_t new_push_index = l_rx_push_index + 1;

	if( new_push_index >= UART_RX_BUFFER_LENGTH )
	new_push_index = 0;

	// drop character if there is no space in the buffer
	if(new_push_index == l_rx_pop_index)
		return;

	// store dosCharacter
	g_rx_buffer[l_rx_push_index] = in_byte;

	// store new index
	l_rx_push_index = new_push_index;
}

#ifdef __AVR_ATmega8__
/*****************************************************************************/
/* ATMEGA8 Processor specific code                                           */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize UART
void UARTInit(void)
{
	UBRRH = HIGH(UART_BAUD_SELECT);
	UBRRL = LOW(UART_BAUD_SELECT);
	UCSRA = 0;
	UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Start transmission (enables TX reg empty interrupt)
static void UARTStartTransmission(void)
{
	// initiate transmission
	UCSRB |= _BV(UDRIE);
}

///////////////////////////////////////////////////////////////////////////////
// UART Transmit Complete Interrupt Function
ISR(USART_UDRE_vect)
{
	if( l_tx_pop_index == l_tx_push_index )
	{
		// if no more dosCharacter, stop UART
		UCSRB &= ~_BV(UDRIE);
	}
	else
	{
		// Send next dosCharacter in string
		UDR = g_tx_buffer[l_tx_pop_index];

		// step to the next dosCharacter
		l_tx_pop_index++;
		if( l_tx_pop_index >= UART_TX_BUFFER_LENGTH )
		l_tx_pop_index = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// UART Receive Complete Interrupt Function
SIGNAL(USART_RXC_vect)
{
	// read dosCharacter
	uint8_t data = UDR;

	UARTStoreReceivedCharacter(data);
}

#endif

#ifdef __AVR_ATmega328P__
/*****************************************************************************/
/* ATMEGA328P Processor specific code                                        */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize UART
void UARTInit(void)
{
	UBRR0H = HIGH(UART_BAUD_SELECT);
	UBRR0L = LOW(UART_BAUD_SELECT);
	UCSR0A = 0;
	UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Start transmission (enables TX reg empty interrupt)
static void UARTStartTransmission(void)
{
	// initiate transmission
	UCSR0B |= _BV(UDRIE0);
}

///////////////////////////////////////////////////////////////////////////////
// UART Transmit Complete Interrupt Function
ISR(USART_UDRE_vect)
{
	if( l_tx_pop_index == l_tx_push_index )
	{
		// if no more dosCharacter, stop UART
		UCSR0B &= ~_BV(UDRIE0);
	}
	else
	{
		// Send next dosCharacter in string
		UDR0 = g_tx_buffer[l_tx_pop_index];

		// step to the next dosCharacter
		l_tx_pop_index++;
		if( l_tx_pop_index >= UART_TX_BUFFER_LENGTH )
		l_tx_pop_index = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// UART Receive Complete Interrupt Function
SIGNAL(USART_RX_vect)
{
	// read dosCharacter
	uint8_t data = UDR0;
	
	UARTStoreReceivedCharacter(data);
}

#endif
