/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOMDEMOCPT_PROCESSOR_H_
#define _WOMDEMOCPT_PROCESSOR_H_
#include <stdio.h>
#include <omxil/OMX_Types.h>
#include <omxil/OMX_Component.h>

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

class womDemoCpt_processor: public Wom_Processor
//*************************************************************************************************************
{
public:
	womDemoCpt_processor(const char *name, void *UsrPtr=NULL);
	virtual ~womDemoCpt_processor();
	virtual void process();
	enum
	{
		eInputPort_Img       = 0,
		eOutputPort_Img      = 1,
		eOutputPort_Metadata = -1,
	};
	virtual int Init(Wom_Component &aComponent, unsigned int NbPort, Interface_ProcessorCB *pInterface);
	virtual int DeInit();
	virtual int Start();
	virtual int Stop();
	virtual int SendAsyncCommand(AsyncMsg &Msg);
	virtual int SetConfig(size_t index, void* opaque_ptr);

protected:
	virtual int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);
};


#endif //_WOMDEMOCPT_PROCESSOR_H_
