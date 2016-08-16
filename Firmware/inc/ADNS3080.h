/*****************************************************************************/
/*                                                                           */
/*    FlowNavi Firmware                                                      */
/*                                                                           */
/*    Copyright (C) 2015 Laszlo Arvai                                        */
/*                                                                           */
/*    ------------------------------------------------------------------     */
/*    ADNS3080 Optical Flow Sensor Driver                                    */
/*****************************************************************************/

#ifndef __ADNS3080_h
#define __ADNS3080_h

///////////////////////////////////////////////////////////////////////////////
// includes
#include <stdint.h>
#include <Helpers.h>

///////////////////////////////////////////////////////////////////////////////
// Constants
#define ADNS3080_SROM_SIZE 1986

#define ADNS3080_HORIZONTAL_RESOLUTION 	30
#define ADNS3080_VERTICAL_RESOLUTION 		30
#define ADNS3080_IMAGE_SIZE (ADNS3080_HORIZONTAL_RESOLUTION * ADNS3080_VERTICAL_RESOLUTION)

///////////////////////////////////////////////////////////////////////////////
// Types

/// Motion data structure
typedef struct 
{
	uint8_t Motion;
	int8_t DeltaX;
	int8_t DeltaY;
	uint8_t SQUAL;
	uint8_t ShutterUpper;
	uint8_t ShutterLower;
	uint8_t MaximumPixel;
	
} ADNS3080MotionData;

/// Frame capture callback function
typedef void (*ADNS3080FrameDataCallback)(uint8_t in_data);

typedef enum
{
	ADNS3080IM_Off,
	ADNS3080IM_On,
	ADNS3080IM_Auto
} ADNS3080IlluminationMode;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes

void ADNS3080Init(void);
bool_t ADNS3080DownloadSROM(void);
void ADNS3080ReadMotionData(ADNS3080MotionData* out_data);
void ADNS3080UpdateIllumination(ADNS3080MotionData* in_motion_data, ADNS3080IlluminationMode in_illumination_mode);


uint8_t ADNS3080ReadSurfaceQuality(void);
void ADNS3080CaptureFrame(ADNS3080FrameDataCallback in_callback);



#endif