/*****************************************************************************/
/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
*/
/*****************************************************************************/
#include "ImgEns_Shared.h"
#include <omxil/OMX_Component.h>
#include "ImgEns_Trace.h"
#include "ImgEns_Component.h"

// Img_TraceBuilder methods
OMX_ERRORTYPE Img_TraceBuilder::createInitialZone() 
{
	mSharedTraceInfoPtr = new ImgEns_TraceInfo;
	if (mSharedTraceInfoPtr == 0) 
	{
		return OMX_ErrorInsufficientResources;
	}

	// initialisation: disable all traces (parentHandle not yet known)
	mSharedTraceInfoPtr->parentHandle = 0;
	mSharedTraceInfoPtr->traceEnable  = 0;
	mSharedTraceInfoPtr->dspAddr      = 0;
#if (defined(ENABLE_TRACE_GROUP_ERROR))
	mSharedTraceInfoPtr->traceEnable |= 0x1;
#endif
#if (defined(ENABLE_TRACE_GROUP_WARNING))
	mSharedTraceInfoPtr->traceEnable |= 0x2;
#endif
#if (defined(ENABLE_TRACE_GROUP_FLOW))
	mSharedTraceInfoPtr->traceEnable |= 0x4;
#endif
#if (defined(ENABLE_TRACE_GROUP_DEBUG))
	mSharedTraceInfoPtr->traceEnable |= 0x8;
#endif
#if (defined(ENABLE_TRACE_GROUP_API))
	mSharedTraceInfoPtr->traceEnable |= 0x10;
#endif
#if (defined(ENABLE_TRACE_GROUP_OMX_API))
	mSharedTraceInfoPtr->traceEnable |= 0x20;
#endif
#if (defined(ENABLE_TRACE_GROUP_OMX_BUFFER))
	mSharedTraceInfoPtr->traceEnable |= 0x40;
#endif
#if (defined(ENABLE_TRACE_GROUP_RESERVED))
	mSharedTraceInfoPtr->traceEnable |= 0x80;
#endif

	return OMX_ErrorNone;
}

void Img_TraceBuilder::setTraceZone(OMX_U32 handle, OMX_U16 traceLevel) 
{
	mSharedTraceInfoPtr->parentHandle = handle;
	mSharedTraceInfoPtr->traceEnable = traceLevel;
}

// TraceBuilderHST methods
void ImgEns_TraceBuilderHost::setTraceZone(OMX_U32 handle, OMX_U16 traceLevel) 
{
	Img_TraceBuilder::setTraceZone(handle,traceLevel);
}

OMX_ERRORTYPE ImgEns_TraceBuilderHost::createExtraZone(t_uint32 /*domainId*/) 
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_TraceBuilderHost::deleteExtraZone() 
{
	return OMX_ErrorNone;
}

static int memoryTraceEnabled = 0;

int isMemoryTraceEnabled() 
{
	return memoryTraceEnabled;
}

extern "C"  
{
	int enableMemoryTrace(int status) 
	{
		memoryTraceEnabled = status;
		return 1;
	}
}

