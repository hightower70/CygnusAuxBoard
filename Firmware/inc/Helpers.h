/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    General helper functions and macros                                    */
/*****************************************************************************/

#ifndef __Helpers_h
#define __Helpers_h

typedef enum { boolFalse = 0, boolTrue = 1 } bool_t;

#ifndef LOW
#define LOW(x) ((x)&0xff)
#endif

#ifndef HIGH
#define HIGH(x) ((x)>>8)
#endif

#ifndef BV
#define BV(x) (1<<(x))
#endif


#endif /* HELPERS_H_ */