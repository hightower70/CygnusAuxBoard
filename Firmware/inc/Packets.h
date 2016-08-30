/*****************************************************************************/
/* Packet structures for communication with the outside world                */
/*                                                                           */
/* Copyright (C) 2016 Laszlo Arvai                                           */
/* All rights reserved.                                                      */
/*                                                                           */
/* This program is free software: you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation, either version 3 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*****************************************************************************/

#ifndef __Packets_h
#define __Packets_h

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdint.h>

/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/

// packet types
#define PT_DISTANCE 1
#define PT_FRAME    2
#define PT_MOTION		3
#define PT_IR				4

/*****************************************************************************/
/* Types                                                                     */
/*****************************************************************************/

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