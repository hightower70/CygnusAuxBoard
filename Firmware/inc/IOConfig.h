/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    I/O Configuration                                                      */
/*****************************************************************************/

#ifndef __IOConfig_h
#define __IOConfig_h

///////////////////////////////////////////////////////////////////////////////
// Common definitions
#define PIN_HIGH 1
#define PIN_LOW 0

///////////////////////////////////////////////////////////////////////////////
// UART Settings
#define UART_BAUD_RATE 115200
#define UART_TX_BUFFER_LENGTH 128
#define UART_RX_BUFFER_LENGTH 64

///////////////////////////////////////////////////////////////////////////////
// ADNS3080 definitions
#define ADNS3080SPIInit() SPIInit()
#define ADNS3080SPIWriteAndReadData(x) SPIWriteAndReadData(x)

#define ADNS3080CSInit()		{ PORTD |= BV(PD7); DDRD |= BV(PD7); }
#define ADNS3080CS(x)       (void)((((x)==0)?(PORTD &= ~BV(PD7)) : (PORTD |= BV(PD7))))

#define ADNS3080RESETInit()	{ DDRB |= BV(PB2); }
#define ADNS3080RESET(x)    (void)((((x)==0)?(PORTB &= ~BV(PB2)) : (PORTB |= BV(PB2))))

#define ADNS3080LEDEnableInit() { DDRD &= ~BV(PD6); PORTD &= ~BV(PD6); }
#define ADNS3080LEDEnable(x)  (void)((((x)==0)?(DDRD |= BV(PD6)) : (DDRD &= ~BV(PD6))))


///////////////////////////////////////////////////////////////////////////////
// PixArt sensor
#define I2C_FREQ 100000
#define IRSENSOR_ADDRESS 0x58 

///////////////////////////////////////////////////////////////////////////////
// Digital sequencer output
#define DIO1_DDR DDRB
#define DIO1_PORT PORTB
#define DIO1_PIN PORTB2

#define DIO2_DDR DDRD
#define DIO2_PORT PORTD
#define DIO2_PIN PORTD6

#define DIO3_DDR DDRD
#define DIO3_PORT PORTD
#define DIO3_PIN PORTD7

///////////////////////////////////////////////////////////////////////////////
// Ultrasonic sensor
#define USEL0_DDR DDRD
#define USEL0_PIN PORTD2
#define USEL0_PORT PORTD

#define USEL1_DDR DDRD
#define USEL1_PIN PORTD3
#define USEL1_PORT PORTD

#define TRIG_DDR DDRB
#define TRIG_PIN PORTB1
#define TRIG_PORT PORTB

#endif 