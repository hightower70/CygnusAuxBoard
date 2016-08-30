/*****************************************************************************/
/* Ultrasonic distance sensor driver                                         */
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

#ifndef __UltrasonicDistance_h
#define __UltrasonicDistance_h

/*****************************************************************************/
/* Types                                                                     */
/*****************************************************************************/
#include <stdint.h>
#include <Helpers.h>

/*****************************************************************************/
/* Types                                                                     */
/*****************************************************************************/

typedef struct
{
	uint16_t Distance;
	uint16_t Temperature;
} USDMeasuredData;

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

void USDInit(void);
void USDProcess(void);
bool_t USDIsDataAvailable(void);
bool_t USDGetMeasuredData(USDMeasuredData* out_data);


#endif