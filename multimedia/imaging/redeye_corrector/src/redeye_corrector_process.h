/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _REDEYE_CORRECTOR_PROCESS_H_
#define _REDEYE_CORRECTOR_PROCESS_H_
#include <stdio.h>
#include <OMX_Types.h>
#include <OMX_Component.h>
#include <Component.h> // because we NMF-extend fsm.component.component
#include <IFM_Types.h>

#include "REC_Api.h"

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


class RedEyeCorrector_proc: public openmax_processor
//*************************************************************************************************************
{
public:
	RedEyeCorrector_proc();
	~RedEyeCorrector_proc();
	virtual void process();

	enum
	{
		eInputPort_Metadata  =0,
		eInputPort_Img       =1,
		eOutputPort_Img      =2,
	};
protected:
	virtual int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);
	virtual int UserOpen();
};

#ifdef __EPOC32__
NONSHARABLE_CLASS(RedEyeCorrector_proc);
#endif


#endif //_REDEYE_CORRECTOR_PROCESS_H_
