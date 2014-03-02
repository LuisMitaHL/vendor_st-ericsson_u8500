/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "wom_shared.h"
#include <omxil/OMX_Types.h>
#include <omxil/OMX_Core.h>
#include "wom_queue.h"
#include "wom_fsm.h"
#include "ImgEns_ProcessingComponent.h"
#include "ImgEns_Component.h"
#include "ImgEns_Port.h"
#include "wom_tools.h"
#include "wom_port.h"
#include "wom_syncproxy.h"

#undef    OMXCOMPONENT
#if (!defined(__SYMBIAN32__))
	#define   OMXCOMPONENT GetComponentName()
#else
	#define   OMXCOMPONENT "Sync_Proxy"
#endif
#define   TRACENAME "Sync_Proxy"

Sync_Proxy::Sync_Proxy(const char *name, ImgEns_Component &comp, Interface_ProcessorToComponent &IProcessorToComponent)
: Img_ProcessingComponent(comp)
, m_Name(name)
, m_UserName(name)
, mI_ProcessorToComponent(IProcessorToComponent)
//*************************************************************************************************************
{
	Debug("Constructor\n");
}


Sync_Proxy::~Sync_Proxy()
//*************************************************************************************************************
{
	Debug("Destructor\n");
}


OMX_ERRORTYPE Sync_Proxy::construct(void)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("construct\n");
	//Simulate command from 'dsp'
	mI_ProcessorToComponent.SendOmxEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle); //eventHandler <- When return from asynchrone cmd
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::destroy(void)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("destroy\n");
	return(status);
}


OMX_ERRORTYPE Sync_Proxy::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorUnsupportedIndex;
	if (nConfigIndex || pComponentConfigStructure){}
	WOM_ASSERT(0);
	Debug("applyConfig\n");
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorUnsupportedIndex;
	if (nConfigIndex || pComponentConfigStructure){}
	WOM_ASSERT(0);
	Debug("retrieveConfig\n");
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("sendCommand eCmd=%d, nData=%d\n", eCmd, nData);
	if (eCmd==OMX_CommandStateSet)
	{	// Il faut appeler la version dediée asynchrone eventHandler et non eventHandlerCB
		mI_ProcessorToComponent.SendOmxEvent((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandStateSet, nData);
		mI_ProcessorToComponent.SendOmxEvent(OMX_EventCmdComplete, OMX_CommandStateSet, nData);
	}
	else
	{	// Il faut appeler la version dediée asynchrone
		mI_ProcessorToComponent.SendOmxEvent((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, eCmd, nData); //First have to acknowledge the cmd
		mI_ProcessorToComponent.SendOmxEvent(OMX_EventCmdComplete, eCmd, nData); // Then said that it is complete
	}
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("emptyThisBuffer InputPortIndex=%d\n", pBuffer->nInputPortIndex);
	pBuffer->nFilledLen=0;
	mI_ProcessorToComponent.EmptyBufferDone(pBuffer);
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("fillThisBuffer nOutputPortIndex=%d\n", pBuffer->nOutputPortIndex);
	pBuffer->nFilledLen=17;
	mI_ProcessorToComponent.FillBufferDone(pBuffer);
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo)
//*************************************************************************************************************
{
	Debug("Allocate buffer PortIndex %lu, nBufferIndex=%lu, nSizeBytes %lu\n", nPortIndex, nBufferIndex, nSizeBytes);
	OMX_ERRORTYPE status=OMX_ErrorNone;
#if (defined(WORKSTATION) || defined(_MSC_VER))
	//Make allocation on the heap
	*ppData = (OMX_U8 *) new char[nSizeBytes];
	if (*ppData  ==NULL)
	{
		WOM_ASSERT(false);
		return OMX_ErrorInsufficientResources;
	}
	*((OMX_U32 **) bufferAllocInfo) = (OMX_U32 *) *ppData ;
	*portPrivateInfo = (void *)NULL;
#else
	WOM_ASSERT(0); //Not implemented today
#endif
	Debug("allocateBuffer PortIndex %lu, nBufferIndex=%lu, nSizeBytes %lu\n", nPortIndex, nBufferIndex, nSizeBytes);
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	int index;
	if (dir == OMX_DirInput)
		index= pBuffer->nInputPortIndex;
	else
		index= pBuffer->nOutputPortIndex;

	Debug("useBufferHeader dir=%s, index=%d \n", dir==OMX_DirInput?"Input":"Output", index);
	return(status);
}


OMX_ERRORTYPE Sync_Proxy::useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	if (pBufferHdr || portPrivateInfo){}
	Debug("useBuffer nPortIndex=%d nBufferIndex=%d\n", nPortIndex, nBufferIndex);
	return(status);
}


OMX_ERRORTYPE Sync_Proxy::freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	if (portPrivateInfo) {}
	Debug( "freeBuffer nPortIndex %lu- nBufferIndex %lu bBufferAllocated=%s\n", nPortIndex, nBufferIndex, bBufferAllocated==OMX_TRUE?"true":"false");
#if (defined(WORKSTATION) || defined(_MSC_VER))
	Debug("freeBuffer nPortIndex=%lu nBufferIndex=%lu bBufferAllocated=%s\n", nPortIndex, nBufferIndex, bBufferAllocated==OMX_TRUE?"true":"false");
	if (bBufferAllocated == OMX_TRUE)
	{
		if (bBufferAllocated)
		{
			delete [] (char *)bufferAllocInfo;
		}
	}
#else
	WOM_ASSERT(0);
	Debug("**** freeBuffer nPortIndex=%lu nBufferIndex=%lu bBufferAllocated=%s is not yet implemented for target\n", nPortIndex, nBufferIndex, bBufferAllocated==OMX_TRUE?"true":"false");
#endif
	return(status);
}


OMX_ERRORTYPE Sync_Proxy::getMMHWBufferInfo(OMX_U32 /*nPortIndex*/, OMX_U32 /*nSizeBytes*/, OMX_U8 * /*pData*/, void ** /*bufferAllocInfo*/, void ** /*portPrivateInfo*/)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("getMMHWBufferInfo is not yet implemented\n");
	return(status);
}

OMX_ERRORTYPE Sync_Proxy::freeSharedBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void * /*bufferAllocInfo*/, void * /*portPrivateInfo*/)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("freeSharedBuffer nPortIndex=%d nBufferIndex=%d bBufferAllocated=%d is not yet implemented\n", nPortIndex, nBufferIndex, bBufferAllocated);
	return(status);
}


void Sync_Proxy::Debug(const char *format, ...) const
//*************************************************************************************************************
{
	va_list list;
	va_start(list, format);
	const char *name=m_Name;
	if ( (name==NULL) || (*name=='\0') )
		name="Sync_Proxy";

#ifndef __SYMBIAN32__
	fprintf (stdout, "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s: ", name);
	vfprintf(stdout, format, list);
#else
	static char DebugString[1024];
	vsprintf(DebugString, format, list);
	RDebug::Printf("\nnew_splitter_Proxy:%s %s", name, DebugString);
#endif
	va_end(list);
}

