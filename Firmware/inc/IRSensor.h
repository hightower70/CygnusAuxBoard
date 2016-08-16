/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    IRSensor driver                                                        */
/*****************************************************************************/

#ifndef __IRSENSOR_h
#define __IRSENSOR_h

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define IRSENSOR_MAX_BLOB_COUNT 4

///////////////////////////////////////////////////////////////////////////////
// Types
typedef struct 
{
	uint16_t x_coordinate;
	uint16_t y_coordinate;
	uint8_t blob_size;
} IRSensorBlobInfo;

typedef struct 
{
	IRSensorBlobInfo blob_info[IRSENSOR_MAX_BLOB_COUNT];
} IRSensorResult;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
void IRSensorSetSensitivity(uint8_t in_sensitivity);
void IRSensorGetResult(IRSensorResult *out_result);

#endif
