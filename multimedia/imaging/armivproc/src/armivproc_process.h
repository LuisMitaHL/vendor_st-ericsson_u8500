/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _ARMIVPROC_PROCESS_H_
#define _ARMIVPROC_PROCESS_H_
#include <stdio.h>
#include <OMX_Types.h>
#include <OMX_Component.h>
#include <Component.h> // because we NMF-extend fsm.component.component
#include <IFM_Types.h>

enum enumError_Armivproc
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
	eError_ArmIVProcEngineIsNull,
	eError_DataStructPtrIsNullForSetConfig,
	eError_WrongOrientationForSetParam,
	eError_WrongRotation,
	eError_WrongMirroringValue,
	//eError_WrongInputFormat,
	//eError_WrongOutputFormat,

	eError_LastEnum_Armivproc,
};

class ArmIVProc_Engine;

class ArmIVProc_process: public openmax_processor
//*************************************************************************************************************
{ // The processor class in Host side
public:
	ArmIVProc_process();
	virtual ~ArmIVProc_process();
	virtual void process(); //Main processing function

	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);

	enum
	{ //Definition of indexes for ports
		eInputPort_Img       = 0,
		eOutputPort_Img      = 1,
		eOutputPort_Metadata = -1, // No ports
	};
	ArmIVProc_Engine *GetEngine() { return(m_pEngine);}

	int Get_OmxRotation() const { return(m_OmxRotation); }
	int Set_OmxRotation(int rot);

	int Set_OmxMirroring(OMX_MIRRORTYPE mirror);


	int ConfigureAlgo(const _tConvertionConfig &Config, eEnumRotation Rotation);
protected:
	virtual int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);
	virtual int UserOpen(); // Called by the framwork when component is ready to 'open' transition from idle ->execute

	ArmIVProc_Engine *m_pEngine;
	int               m_OmxRotation;
	eEnumMirror       m_Mirroring;

	 char             m_UserName[32];

	static ReferenceCount m_Count;
};


enum_ImageFormat GetFormat(OMX_COLOR_FORMATTYPE format);


#ifdef AIQ_PERFTRACE_ENABLE
	#include "AIQ_PerfTracer.h"
#else
	#define AIQ_StartPerfCounter(a)   (void) 0
	#define AIQ_StopPerfCounter(a, b) (void) 0
	#define AIQ_InitPerfTrace(a)      (void) 0
	#define AIQ_DeInitPerfTrace()     (void) 0

#endif

#endif //_ARMIVPROC_PROCESS_H_
