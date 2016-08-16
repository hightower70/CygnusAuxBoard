/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    System timer (1ms) driver                                              */
/*****************************************************************************/


#ifndef __Timer_h
#define __Timer_h

///////////////////////////////////////////////////////////////////////////////
// Includes
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
void TIMERInit(void);
uint16_t TIMERGetTime(void);
void TIMERDelay(uint16_t in_delay_ms);
uint16_t TIMERGetTimeSince(uint16_t in_start_time_ms);

#endif