/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    IRSensor driver                                                        */
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <avr/pgmspace.h>
#include <IRSensor.h>
#include <IOConfig.h>
#include <I2C.h>
#include <Timer.h>

///////////////////////////////////////////////////////////////////////////////
// Types
typedef enum
{
	IRSENSOR_MODE_INACTIVE,
	IRSENSOR_MODE_DELAY,
	IRSENSOR_MODE_COMMAND,
	IRSENSOR_MODE_DATA
} IRSensorMode;

///////////////////////////////////////////////////////////////////////////////
// Module global variables

// Default sensitivity initialization data
static const uint8_t l_default_init_data[] PROGMEM =
{
	// length, data1, data2
	0x02, 0x30, 0x01,
	0x02, 0x30, 0x08,
	0x02, 0x06, 0x90,
	0x02, 0x08, 0xC0,
	0x02, 0x1A, 0x40,
	0x02, 0x33, 0x33,
	0x00
};

static const uint8_t l_level1_init_data[] PROGMEM =
{
	0x02, 0x30, 0x01,
	0x08, 0x00, 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x72,
	0x03, 0x07, 0x00, 0x20,
	0x03, 0x1A, 0x1f, 0x03,
	0x02, 0x33, 0x03,
	0x02, 0x30, 0x08,
	0x00
};

static const uint8_t l_level2_init_data[] PROGMEM =
{
	0x02, 0x30, 0x01,
	0x08, 0x00, 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xc8,
	0x03, 0x07, 0x00, 0x36,
	0x03, 0x1A, 0x35, 0x03,
	0x02, 0x33, 0x03,
	0x02, 0x30, 0x08,
	0x00
};

static const uint8_t l_level3_init_data[] PROGMEM =
{
	0x02, 0x30, 0x01,
	0x08, 0x00, 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xAA,
	0x03, 0x07, 0x00, 0x64,
	0x03, 0x1A, 0x63, 0x03,
	0x02, 0x33, 0x03,
	0x02, 0x30, 0x08,
	0x00
};

static const uint8_t l_level4_init_data[] PROGMEM =
{
	0x02, 0x30, 0x01,
	0x08, 0x00, 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x96,
	0x03, 0x07, 0x00, 0xB4,
	0x03, 0x1A, 0xB3, 0x04,
	0x02, 0x33, 0x03,
	0x02, 0x30, 0x08,
	0x00
};

static const uint8_t l_level5_init_data[] PROGMEM =
{
	0x02, 0x30, 0x01,
	0x08, 0x00, 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x96,
	0x03, 0x07, 0x00, 0xfe,
	0x03, 0x1A, 0xfe, 0x05,
	0x02, 0x33, 0x03,
	0x02, 0x30, 0x08,
	0x00
};

static const uint8_t*  l_init_data[] =
{
	l_default_init_data,
	l_level1_init_data,
	l_level2_init_data,
	l_level3_init_data,
	l_level4_init_data,
	l_level5_init_data
};


static IRSensorMode l_mode = IRSENSOR_MODE_INACTIVE;

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize IR sensor
void IRSensorSetSensitivity(uint8_t in_sensitivity)
{
	const uint8_t* init_data_buffer;
	uint8_t buffer[16];
	uint8_t i;
	uint8_t len;

	init_data_buffer = l_init_data[in_sensitivity];

	len = pgm_read_byte(init_data_buffer);
	while(len > 0)
	{
		i = 0;
		init_data_buffer++;
		while(i < len)
		{
			buffer[i++] = pgm_read_byte(init_data_buffer++);
		}

		I2CStartWriteBlock(IRSENSOR_ADDRESS, buffer, len);

		I2CWaitForReady();

		TIMERDelay(3);

		len = pgm_read_byte(init_data_buffer);
	}
}

void IRSensorGetResult(IRSensorResult *out_result)
{
	uint8_t buffer[16];

	// send command
	buffer[0] = 0x36;
	I2CStartWriteBlock(IRSENSOR_ADDRESS, buffer, 1);
	I2CWaitForReady();

	I2CStartReadBlock(IRSENSOR_ADDRESS, buffer, 16);
	I2CWaitForReady();
	
	// decode result
  out_result->blob_info[0].x_coordinate = buffer[1];
  out_result->blob_info[0].y_coordinate = buffer[2];
  out_result->blob_info[0].blob_size		= (buffer[3] & 0x0f);
  out_result->blob_info[0].x_coordinate += (buffer[3] & 0x30) <<4;
  out_result->blob_info[0].y_coordinate += (buffer[3] & 0xC0) <<2;

  out_result->blob_info[1].x_coordinate = buffer[4];
  out_result->blob_info[1].y_coordinate = buffer[5];
  out_result->blob_info[1].blob_size		= (buffer[6] & 0x0f);
  out_result->blob_info[1].x_coordinate += (buffer[6] & 0x30) <<4;
  out_result->blob_info[1].y_coordinate += (buffer[6] & 0xC0) <<2;

  out_result->blob_info[2].x_coordinate = buffer[7];
  out_result->blob_info[2].y_coordinate = buffer[8];
  out_result->blob_info[2].blob_size		= (buffer[9] & 0x0f);
  out_result->blob_info[2].x_coordinate += (buffer[9] & 0x30) <<4;
  out_result->blob_info[2].y_coordinate += (buffer[9] & 0xC0) <<2;

	out_result->blob_info[3].x_coordinate = buffer[10];
	out_result->blob_info[3].y_coordinate = buffer[11];
	out_result->blob_info[3].blob_size		= (buffer[12] & 0x0f);
	out_result->blob_info[3].x_coordinate += (buffer[12] & 0x30) <<4;
	out_result->blob_info[3].y_coordinate += (buffer[12] & 0xC0) <<2;
}
