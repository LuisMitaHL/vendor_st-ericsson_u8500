/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMAGECONVERTER_LIB_H
#define _IMAGECONVERTER_LIB_H

#ifdef _MSC_VER
	#define IMAGECONVERTER_API  __declspec(dllimport)
#else
	#define IMAGECONVERTER_API  
#endif

#ifndef _SHAREDTOOLS_H_
	#include "SharedTools.h"
#endif

#ifndef _IMAGETOOLS_H_
	#include "ImageTools.h"
#endif

#ifndef _IMAGEBUFFER_H_
	#include "ImageBuffer.h"
#endif

#ifndef _IMAGESTREAM_H
	#include "ImageStream.h"
#endif

#ifndef _IMAGECONVERTER_H
	#include "ImageConverter.h"
#endif

#ifndef _ARMIVPROC_ENGINE_H
	#include "Armivproc_engine.h"
#endif



#endif // End of #ifndef _IMAGECONVERTER_LIB_H
