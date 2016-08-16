/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    Ultrasonic distance sensor driver                                      */
/*****************************************************************************/

#ifndef __UltrasonicDistance_h
#define __UltrasonicDistance_h

///////////////////////////////////////////////////////////////////////////////
// Types
#include <stdint.h>
#include <Helpers.h>

///////////////////////////////////////////////////////////////////////////////
// Types
typedef struct
{
	uint16_t Distance;
	uint16_t Temperature;
} USDMeasuredData;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
void USDInit(void);
void USDProcess(void);
bool_t USDIsDataAvailable(void);
bool_t USDGetMeasuredData(USDMeasuredData* out_data);


#endif