/*****************************************************************************/
/* ADC driver                                                                */
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

#ifndef __ADC_h
#define __ADC_h

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdint.h>

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void ADCinit(void);

/*****************************************************************************/
/* Global variables                                                          */
/*****************************************************************************/
extern uint16_t g_temperature;

#endif