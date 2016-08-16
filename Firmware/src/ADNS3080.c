/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    ADNS3080 Optical Flow Sensor Driver                                    */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// includes
#include <ADNS3080.h>
#include <SPI.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>	//for microsecond delay
#include <IOConfig.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define ADNS3080_REGISTER_WRITE_FLAG		0x80

// Register Map for the ADNS3080 Optical OpticalFlow Sensor
#define ADNS3080_PRODUCT_ID							0x00
#define ADNS3080_REVISION_ID						0x01
#define ADNS3080_MOTION									0x02
#define ADNS3080_DELTA_X								0x03
#define ADNS3080_DELTA_Y								0x04
#define ADNS3080_SQUAL									0x05
#define ADNS3080_PIXEL_SUM							0x06
#define ADNS3080_MAXIMUM_PIXEL					0x07
#define ADNS3080_CONFIGURATION_BITS			0x0a
#define ADNS3080_EXTENDED_CONFIG				0x0b
#define ADNS3080_DATA_OUT_LOWER					0x0c
#define ADNS3080_DATA_OUT_UPPER					0x0d
#define ADNS3080_SHUTTER_LOWER					0x0e
#define ADNS3080_SHUTTER_UPPER					0x0f
#define ADNS3080_FRAME_PERIOD_LOWER			0x10
#define ADNS3080_FRAME_PERIOD_UPPER			0x11
#define ADNS3080_MOTION_CLEAR						0x12
#define ADNS3080_FRAME_CAPTURE					0x13
#define ADNS3080_SROM_ENABLE						0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER      0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER      0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER      0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER      0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER           0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER           0x1e
#define ADNS3080_SROM_ID								0x1f
#define ADNS3080_OBSERVATION						0x3d
#define ADNS3080_INVERSE_PRODUCT_ID			0x3f
#define ADNS3080_PIXEL_BURST						0x40
#define ADNS3080_MOTION_BURST						0x50
#define ADNS3080_SROM_LOAD							0x60

// Config register bits
#define ADNS3080_CONF_LED_MODE_ALWAYS_ON        0x00
#define ADNS3080_CONF_LED_MODE_WHEN_REQUIRED    BV(6)

#define ADNS3080_CONF_RESOLUTION_400			0x00
#define ADNS3080_CONF_RESOLUTION_1600		BV(4)

///////////////////////////////////////////////////////////////////////////////
// Local functions
static uint8_t ADNS3080ReadRegister(uint8_t in_address);
static void ADNS3080WriteRegister(uint8_t in_address, uint8_t in_data);

///////////////////////////////////////////////////////////////////////////////
// Global variables
extern const uint8_t g_avago_srom[] PROGMEM;

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize ADNS3080 sensor
void ADNS3080Init(void)
{
	// init IO
	ADNS3080RESETInit();
	ADNS3080CSInit();
	ADNS3080SPIInit();
	ADNS3080LEDEnableInit();

	ADNS3080LEDEnable(0);

	// Pulse reset high for 10 us
	ADNS3080RESET(PIN_HIGH);
	_delay_us(20); // > 10us
	ADNS3080RESET(PIN_LOW);
	_delay_us(600); // > 500us
	
	//Send SROM to sensor
	ADNS3080DownloadSROM();

	// Set config register
	ADNS3080WriteRegister(ADNS3080_CONFIGURATION_BITS, ADNS3080_CONF_LED_MODE_WHEN_REQUIRED | ADNS3080_CONF_RESOLUTION_400);
	
	//Disable serial port pull-ups
	ADNS3080WriteRegister(ADNS3080_EXTENDED_CONFIG, 0b00000100);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Downloads the SROM data to the specified sensor
/// @return True if download was success
bool_t ADNS3080DownloadSROM(void)
{
	char valid;
	uint16_t pos;

	//Toggle reset
	ADNS3080RESET(PIN_HIGH);
	_delay_us(10);	// > 10us
	ADNS3080RESET(PIN_LOW);
	_delay_us(600); // > 500us

	//Write 0x44 to reg 0x20
	ADNS3080WriteRegister(0x20, 0x44);
	_delay_us(150);

	//Write 0x07 to reg 0x23
	ADNS3080WriteRegister(0x23, 0x07);
	_delay_us(150);

	//Write 0x88 to register 0x24
	ADNS3080WriteRegister(0x24, 0x88);
	//Wait >= 1 frame period
	_delay_us(500);

	//Write 0x18 to register 0x14 (SROM_Enable register)
	ADNS3080WriteRegister(ADNS3080_SROM_ENABLE, 0x18);
	_delay_us(150);
	
	//Begin burst mode write of data file to register 0x60 (SROM_Load register)
	ADNS3080CS(PIN_LOW);

	ADNS3080SPIWriteAndReadData(ADNS3080_REGISTER_WRITE_FLAG | ADNS3080_SROM_LOAD);
	_delay_us(10);

	for(pos = 0; pos < ADNS3080_SROM_SIZE; pos++)
	{
		ADNS3080SPIWriteAndReadData(pgm_read_byte(&g_avago_srom[pos]));
		_delay_us(10);
	}
	ADNS3080CS(PIN_HIGH);

	_delay_us(150);

	//Read the SROM_ID reg to verify successful SROM download
	valid = ADNS3080ReadRegister(ADNS3080_SROM_ID);
	
	//Returns the contents of the SROM_ID reg
	return valid;
}


///////////////////////////////////////////////////////////////////////////////
/// @brief Reads motion data
/// @param [out] out_data Data struct to fill with motion information
void ADNS3080ReadMotionData(ADNS3080MotionData* out_data)
{
	// Start communication
	ADNS3080CS(PIN_LOW);

	// start reading
	ADNS3080SPIWriteAndReadData(ADNS3080_MOTION_BURST);
	_delay_us(75);

	// read all 7 bytes (Motion, Delta_X, Delta_Y, SQUAL, Shutter_Upper, Shutter_Lower, Maximum Pixels)
	out_data->Motion = ADNS3080SPIWriteAndReadData(0);
	out_data->DeltaX = ADNS3080SPIWriteAndReadData(0);
	out_data->DeltaY = ADNS3080SPIWriteAndReadData(0);
	out_data->SQUAL = ADNS3080SPIWriteAndReadData(0);
	out_data->ShutterUpper = ADNS3080SPIWriteAndReadData(0);
	out_data->ShutterLower = ADNS3080SPIWriteAndReadData(0);
	out_data->MaximumPixel = ADNS3080SPIWriteAndReadData(0);

	ADNS3080CS(PIN_HIGH);

	_delay_us(10);
}

static uint16_t l_maximum_pixel_sum;
static uint8_t l_maximum_pixel_sum_count;


void ADNS3080UpdateIllumination(ADNS3080MotionData* in_motion_data, ADNS3080IlluminationMode in_illumination_mode)
{
	switch(in_illumination_mode)
	{
		case ADNS3080IM_Off:
			ADNS3080LEDEnable(0);
			break;

		case ADNS3080IM_On:
			ADNS3080LEDEnable(1);
			break;
	}
	/*
	l_maximum_pixel_sum += in_motion_data->MaximumPixel;
	l_maximum_pixel_sum_count++;

	if(l_maximum_pixel_sum_count == 50)
	{
		l_maximum_pixel_sum /= 50;

		if(l_maximum_pixel_sum < 5)
		{
			ADNS3080LEDEnable(1);
		}
		else
		{
			if(l_maximum_pixel_sum > 55)
			{
				ADNS3080LEDEnable(0);
			}
		}

		l_maximum_pixel_sum = 0;
		l_maximum_pixel_sum_count = 0;
	}*/
}


///////////////////////////////////////////////////////////////////////////////
/// @brief Capture frame data
/// @param in_callback Frame data processing callback function pointer
void ADNS3080CaptureFrame(ADNS3080FrameDataCallback in_callback)
{
	uint8_t data;

	// capture frame
	ADNS3080WriteRegister(ADNS3080_FRAME_CAPTURE, 0x83);
	_delay_us(1510);	// wait t_CAPTURE

	// Start burst read
	ADNS3080CS(PIN_LOW);

	ADNS3080SPIWriteAndReadData(ADNS3080_PIXEL_BURST);
	_delay_us(50);		// wait t_SRAD
	
	for(int i=0; i<ADNS3080_HORIZONTAL_RESOLUTION * ADNS3080_VERTICAL_RESOLUTION; i++)	// max. 1536 Pixels
	{
		data = ADNS3080SPIWriteAndReadData(0x00);
		in_callback(data);
		_delay_us(10);	// wait t_LOAD
	}
	
	ADNS3080CS(PIN_HIGH);

	_delay_us(4);	// wait t_BEXIT
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Reads surface quality value from the sensor
/// @return Quality 0 (no surface) - 169 (best surface)
uint8_t ADNS3080ReadSurfaceQuality(void)
{
	return ADNS3080ReadRegister(ADNS3080_SQUAL);
}

/*****************************************************************************/
/* Local functions                                                           */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// @brief Writes sensor register
/// @param in_address Register address to write
/// @param in_data Data to write into the register
static void ADNS3080WriteRegister(uint8_t in_address, uint8_t in_data)
{
	// start transmission
	ADNS3080CS(PIN_LOW);

	ADNS3080SPIWriteAndReadData( ADNS3080_REGISTER_WRITE_FLAG | in_address);
	ADNS3080SPIWriteAndReadData(in_data);

	ADNS3080CS(PIN_HIGH);

	_delay_us(50);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Reads sensor register
/// @param in_address Register address to read
/// @return Register value
static uint8_t ADNS3080ReadRegister(uint8_t in_address)
{
	uint8_t data;

	// start transmission
	ADNS3080CS(PIN_LOW);

	ADNS3080SPIWriteAndReadData(in_address);
	_delay_us(50);
	data = ADNS3080SPIWriteAndReadData(0x00);

	ADNS3080CS(PIN_HIGH);

	_delay_us(50);

	return data;
}
