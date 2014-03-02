/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef __REDEYEDETECTOR_PROCESS_H__
#define __REDEYEDETECTOR_PROCESS_H__
#include <stdio.h>
#include <OMX_Types.h>
#include <OMX_Component.h>
#include <Component.h> // because we NMF-extend fsm.component.component
#include <IFM_Types.h>
#include "OMX_extensions/RED_OMX_extensions.h"
#include "RED_Api.h"

enum
{
	eError_None =0,
	eError_UnknowFlipMode,
	eError_WrongAlgoID,
	eError_InputAndOutputFormatDifferent,
	eError_UnsupportedFormat,
	eError_SizesAreDifferent,
	eError_WrongOuptutSize,
	eError_MetadataPtrIsNull,
	eError_MissingParam1,
	eError_MissingParam2,
	eError_MissingParam3,
	eError_MissingParam4,
};



class RedEyeDetector_proc: public openmax_processor
//*************************************************************************************************************
{
public:
	RedEyeDetector_proc();
	virtual ~RedEyeDetector_proc();
	virtual void process();
	enum
	{
		eInputPort_Img       =0,
		eOutputPort_Metadata =1,
	};
protected:
	virtual int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);
};

#ifdef __EPOC32__
NONSHARABLE_CLASS(RedEyeDetector_proc);
#endif


#endif //__REDEYEDETECTOR_PROCESS_H__
