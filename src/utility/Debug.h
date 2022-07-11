/*****************************************************************************
* | File      	:	Debug.h
* | Author      :   Waveshare team
* | Function    :	debug with printf
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/
#ifndef _DEBUG_H
#define _DEBUG_H

#include <Wire.h>

#define USE_DEBUG 0
#if USE_DEBUG
	#define Debug(__info) Serial.println(__info)
#else
	#define Debug(__info)  
#endif

#endif

