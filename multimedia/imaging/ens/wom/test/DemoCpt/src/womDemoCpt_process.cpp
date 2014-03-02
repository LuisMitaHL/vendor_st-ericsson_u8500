/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "womDemoCpt_shared.h"
#include <stdio.h>
#include <string.h>

#undef  OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

#undef    OMXCOMPONENT
#define   OMXCOMPONENT "womDemoCpt_processor"
#include "osi_trace.h"
//#include "debug_trace.h"
#include "wom_lib.h"
#include "womDemoCpt_proxy.h"
#include "womDemoCpt_process.h"

#include "../algo/api/womDemoCpt_api.h" 

#undef    OMXCOMPONENT
#ifdef __SYMBIAN32__
	#define   OMXCOMPONENT "womDemoCpt_process"
#else
	#define   OMXCOMPONENT GetComponentName()
#endif

womDemoCpt_processor::womDemoCpt_processor(const char *name, void *UsrPtr)
: Wom_Processor(name, UsrPtr)
//*************************************************************************************************************
{ 
	IN0("\n");
	MSG1("Construction of womDemoCpt_process(%s)\n", m_UserName);
#if (OMX_BUFFER_SHARING!=0)
	//Make initialisation of buffer sharing
	m_bUseBufferSharing=true;
#endif
	OUT0("\n");
}

womDemoCpt_processor::~womDemoCpt_processor()
//*************************************************************************************************************
{ 
	IN0("\n");
	OUT0("\n");
}

int womDemoCpt_processor::Init(Wom_Component &aComponent, unsigned int NbPort, Interface_ProcessorCB *pInterface)
//*************************************************************************************************************
{ // replace nmf start + fsm_init
	int res= Wom_Processor::Init(aComponent, NbPort, pInterface);
	return(res);
}

int womDemoCpt_processor::DeInit()
//*************************************************************************************************************
{ // replace nmf stop, call juste at the end
	int res= Wom_Processor::DeInit();
	return(res);
}

int womDemoCpt_processor::Start()
//*************************************************************************************************************
{ // When component is ready to start
	int res= Wom_Processor::Start();
	return(res);
}

int womDemoCpt_processor::Stop()
//*************************************************************************************************************
{ // When component is ready to stop
	int res= Wom_Processor::Stop();
	return(res);
}

int womDemoCpt_processor::SendAsyncCommand(AsyncMsg &Msg)
//*************************************************************************************************************
{ // When component received a specific command
	int res= Wom_Processor::SendAsyncCommand(Msg);
	return(res);
}

int womDemoCpt_processor::SetConfig(size_t index, void* opaque_ptr)
//*************************************************************************************************************
{ // By default unsupported index
	int res=S_OK;
	// setMyConfig( index, opaque_ptr);
	return(res);
}

//Warning this function can only by called when there is only one input and one output
int womDemoCpt_processor::GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue)
//*************************************************************************************************************
{ //This method is called for preparing default ConvertionConfig struct
	IN0("\n");
	int res=InitProcessingInfo(Config, bDequeue, eInputPort_Img, eOutputPort_Img, eOutputPort_Metadata);
	OUTR("", res);
	return(res);
}

void womDemoCpt_processor::process()
//*************************************************************************************************************
{ // Process IMG to Metadata
	IN0("\n");
	const int InputIndex  = eInputPort_Img;
	const int OutputIndex = eOutputPort_Img;
	_tConvertionConfig Config;
	int res;
	int inBufCount;
	int outBufCount;

	if (m_bUseBufferSharing==false)
	{ // No buffer sharing 
		//Check that there are at least on input and one ouptut buffer
		inBufCount  = GetPort(InputIndex)->queuedBufferCount();
		outBufCount = GetPort(OutputIndex)->queuedBufferCount();
		MSG3("%s inBufCount %d, outBufCount %d\n", GetComponentName(), inBufCount, outBufCount);
		if ((inBufCount==0)  || (outBufCount==0))
		{
			OUT0("\n");
			return; //No buffer to process
		}

		// Input and output buffers are available: can do the processing
		res=InitProcessingInfo(Config, true, InputIndex, OutputIndex, -1);
		
		// Uncomment following lines if you want to get some extradata
		//OMX_EXTRADATATYPE *pData;
		//if (GetExtraData(Config.Input.pBufferHdr, OMX_SYMBIAN_CameraExtraDataCaptureParameters, (char*&)pData )==S_OK)
		//{
		//}

		Config.Output.pBufferHdr->nFilledLen = Config.Input .pBufferHdr->nFilledLen; //Update output size
		Config.Output.pBufferHdr->nOffset    = 0;
		Config.Output.pBufferHdr->nFlags     = Config.Input .pBufferHdr->nFlags;     //Update output flags

		//Now we can call our processing function
		res= m_fn_UserProcessBuffer(Config); // Call specific processing
		if (res!=0)
		{ // should implement error handling
		}

		if((Config.Input.pBufferHdr->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK)!=0)
		{ //Input buffer is marked with End of stream flag, recopy of the flag is needed
			portInformation *pPortInfo;
			/* Uncomment this code if the component sink and don't produce output buffer
			pPortInfo=GetPortInfo(InputIndex);
			if (pPortInfo)
			{
				m_pIProcessorCB->OmxEventFeedback(OMX_EventBufferFlag, pPortInfo->omxPortIndex, Config.Input.pBufferHdr->nFlags);
			} */
			pPortInfo=GetPortInfo(OutputIndex);
			if (pPortInfo)
			{
				m_pIProcessorCB->OmxEventFeedback(OMX_EventBufferFlag, pPortInfo->omxPortIndex, (Config.Input.pBufferHdr->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK));
			}
		}
		else
		{
		}
		

		MSG0("Copy ExtraData\n");
		//CopyExtraData(Config);

		Config.Input .pBufferHdr->nFilledLen = 0; // reset processed buffer
		//release processed buffer
		GetPort(InputIndex) ->returnBuffer(Config.Input .pBufferHdr);
		GetPort(OutputIndex)->returnBuffer(Config.Output.pBufferHdr);
		// Must add metadata release
	}
	else
	{ //Buffer sharing case 
		//Check that there are at least on input and one ouptut buffer
		inBufCount  = GetPort(InputIndex)      ->queuedBufferCount();
		MSG1("Buffer count: In0 %d\n", inBufCount);

		if (inBufCount==0)
		{
			OUT0("\n");
			return; //No buffer to process
		}

		// Input 0 and 1  and output buffers are available: can do the processing
		//No extradata to process
		res=InitProcessingInfo(Config, true, InputIndex, -1, -1);


		InitBufferInfo(Config.Output, InputIndex, false);
		Config.Output.pBufferHdr=Config.Input.pBufferHdr; //Set output buffer header ptr = to input one
		//Now we can call our processing function
		res= m_fn_UserProcessBuffer(Config); // Call specific processing
		if (res!=0)
		{ // should implement error handling
		}

		if((Config.Input.pBufferHdr->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK)!=0)
		{ //Input buffer is marked with End of stream flag
			portInformation *pPortInfo=GetPortInfo(OutputIndex);
			if (pPortInfo)
			{
				MSG2(" womDemoCpt[d] flags=0x%x. Send it to %d\n", (int)pPortInfo->omxPortIndex, (int)Config.Input.pBufferHdr->nFlags);
				m_pIProcessorCB->OmxEventFeedback(OMX_EventBufferFlag, pPortInfo->omxPortIndex, (Config.Input.pBufferHdr->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK));
			}
		}

		MSG2("Buffer count: Img0 %p, Out=%p\n", Config.Input .pBufferHdr, Config.Output.pBufferHdr);
		GetPort(InputIndex)     ->returnBuffer(Config.Input .pBufferHdr);
	}
	OUT0("\n");
}

