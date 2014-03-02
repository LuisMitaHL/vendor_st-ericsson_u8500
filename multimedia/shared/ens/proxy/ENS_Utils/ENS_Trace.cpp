/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Trace.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "OMX_Component.h"
#include "ENS_Component.h"
#include "ENS_Trace.h"
#include "ENS_Nmf.h"

// TraceBuilder methods
OMX_ERRORTYPE TraceBuilder::createInitialZone() {
	mSharedTraceInfoPtr = new TraceInfo_t;
	if (mSharedTraceInfoPtr == 0) {
		return OMX_ErrorInsufficientResources;
	}

	// initialisation: disable all traces (parentHandle not yet known)
	mSharedTraceInfoPtr->parentHandle = 0;  
  mSharedTraceInfoPtr->traceEnable = 0;  

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
#if (defined(ENABLE_TRACE_GROUP_ALWAYS))
    mSharedTraceInfoPtr->traceEnable |= 0x80;  
#endif

	return OMX_ErrorNone;
}

void TraceBuilder::setTraceZone(OMX_U32 handle, OMX_U16 traceLevel) {
	mSharedTraceInfoPtr->parentHandle = handle;
	mSharedTraceInfoPtr->traceEnable = traceLevel;  
}

// TraceBuilderHST methods
void TraceBuilderHost::setTraceZone(OMX_U32 handle, OMX_U16 traceLevel) {
	TraceBuilder::setTraceZone(handle,traceLevel); 
}

OMX_ERRORTYPE TraceBuilderHost::createExtraZone(t_uint32 domainId) {
	return OMX_ErrorNone;
}

OMX_ERRORTYPE TraceBuilderHost::deleteExtraZone() {
	return OMX_ErrorNone;
}
#ifndef HOST_ONLY
// TraceBuilderMPC methods
void TraceBuilderMpc::updateExtraZone(OMX_U32 handle, OMX_U16 traceLevel) {
	// check extra zone has been created
	if (mExtraZonePtr != 0) {
		mExtraZonePtr->parentHandleL = (handle & 0xFFFF0000UL) >> 16;
		mExtraZonePtr->parentHandleH =  handle & 0x0000FFFFUL;

    mExtraZonePtr->traceEnable = traceLevel;
	}
}

void TraceBuilderMpc::setTraceZone(OMX_U32 handle, OMX_U16 traceLevel) {
	TraceBuilder::setTraceZone(handle,traceLevel);
	updateExtraZone(handle,traceLevel);
}

OMX_ERRORTYPE TraceBuilderMpc::createExtraZone(t_uint32 domainId) {
    t_cm_error error;
    t_cm_system_address SYSmem;

    MEMORY_TRACE_ENTER("TraceBuilderMpc::createExtraZone");

	// create
	error = ENS::allocMpcMemory(domainId, CM_MM_MPC_SDRAM16, sizeof(TraceInfoDspCreate_new_t)/2, CM_MM_ALIGN_2WORDS, &mHandle);
	if (error != CM_OK) {
		return OMX_ErrorInsufficientResources;
	}

    error = CM_GetMpcMemorySystemAddress(mHandle, &SYSmem);
	if (error != CM_OK) {
		return OMX_ErrorUndefined;
	}
	mExtraZonePtr = (TraceInfoDspCreate_new_t *)SYSmem.logical;
	

    error = CM_GetMpcMemoryMpcAddress(mHandle, &(mSharedTraceInfoPtr->dspAddr));
    if (error != CM_OK) {
		return OMX_ErrorUndefined;
	}

	// initialize
	updateExtraZone(mSharedTraceInfoPtr->parentHandle, mSharedTraceInfoPtr->traceEnable);

    MEMORY_TRACE_LEAVE("TraceBuilderMpc::createExtraZone");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE TraceBuilderMpc::deleteExtraZone() {
    if (mHandle) {
	    ENS::freeMpcMemory(mHandle); 
    }
    mExtraZonePtr = 0;
	return OMX_ErrorNone;
}
#endif

static int memoryTraceEnabled = 0;

int isMemoryTraceEnabled() {
    return memoryTraceEnabled;
}

extern "C"  {
int enableMemoryTrace(int status) {
    memoryTraceEnabled = status;
    return 1;
}
}

