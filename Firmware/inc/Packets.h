/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    Packet structures for communication with the outside world             */
/*****************************************************************************/

#ifndef __Packets_h
#define __Packets_h

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// Const

// packet types
#define PT_DISTANCE 1
#define PT_FRAME    2
#define PT_MOTION		3
#define PT_IR				4

///////////////////////////////////////////////////////////////////////////////
// Types

// Flow navi packets

// Ultrasonic distance measurement data packet
typedef struct __attribute__ ((packed))
{
	uint8_t Type;
	uint16_t Distance;
	uint16_t Temperature;
} FNPDistance;

// Motion data packet
typedef struct __attribute__ ((packed))
{
	uint8_t Type;
	uint8_t Motion;
	uint8_t DeltaX;
	uint8_t DeltaY;
	uint8_t Squal;
	uint8_t ShutterUpper;
	uint8_t ShutterLower;
	uint8_t MaxPixel;
} FNPMotion;

// IR sensor data packet entry
typedef struct __attribute__ ((packed))
{
	uint16_t XCoordinate;
	uint16_t YCoordinate;
	uint8_t BlobSize;
} FNPIRDataEntry;

// IR sensor data packet
typedef struct __attribute__ ((packed))
{
	uint8_t Type;
	FNPIRDataEntry BlobInfo[4];
} FNPIRData;

#endif 