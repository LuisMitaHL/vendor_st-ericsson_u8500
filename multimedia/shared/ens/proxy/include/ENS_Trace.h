/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Trace.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_TRACE_H_
#define _ENS_TRACE_H_

#include "ENS_Redefine_Class.h"

#include "OMX_Component.h"

#include <cm/inc/cm.hpp>
#include <inc/type.h>
#include "host/trace.idt.h"

typedef struct {
	t_uint16 parentHandleH;
	t_uint16 parentHandleL;
	t_uint16 traceEnable;
} TraceInfoDspCreate_new_t;


class TraceBuilder {
	public:
	TraceInfo_t * getSharedTraceInfoPtr(void) {return mSharedTraceInfoPtr; };

	virtual void  setTraceZone(OMX_U32 handle, OMX_U16 traceLevel);
	OMX_ERRORTYPE createInitialZone();
	virtual OMX_ERRORTYPE createExtraZone(t_uint32 domainId) = 0;
	virtual OMX_ERRORTYPE deleteExtraZone() = 0;

	virtual ~TraceBuilder() { delete mSharedTraceInfoPtr; };

	protected:
	TraceInfo_t * mSharedTraceInfoPtr;
};

class TraceBuilderHost: public TraceBuilder {
	public:

	virtual void setTraceZone(OMX_U32 handle, OMX_U16 traceLevel);	

	virtual OMX_ERRORTYPE createExtraZone(OMX_U32 domainId);
	virtual OMX_ERRORTYPE deleteExtraZone();

};

#ifndef HOST_ONLY
class TraceBuilderMpc: public TraceBuilder {
	public:

 TraceBuilderMpc() : mHandle(0), mExtraZonePtr((TraceInfoDspCreate_new_t *)NULL) {} ;

	virtual void setTraceZone(OMX_U32 handle, OMX_U16 traceLevel);	

	virtual OMX_ERRORTYPE createExtraZone(OMX_U32 domainId); 
	virtual OMX_ERRORTYPE deleteExtraZone();
	void updateExtraZone(OMX_U32 handle, OMX_U16 traceLevel);	

	private:
    t_cm_memory_handle mHandle;
	TraceInfoDspCreate_new_t * mExtraZonePtr;

};
#endif

extern "C" {
#ifndef HOST_ONLY
    int printf(const char *fmt, ...);
    int sprintf(char * buf, const char *fmt, ...);
#endif //ifndef HOST_ONLY

    int enableMemoryTrace(int status);
}

int isMemoryTraceEnabled();
void traceMemoryStatus();

#define MEMORY_TRACE_ENTER(method) \
    if (isMemoryTraceEnabled()) { \
        printf("***>" method "\n"); \
    }

#define MEMORY_TRACE_LEAVE(method) \
    if (isMemoryTraceEnabled()) { \
        printf("***<" method "\n"); \
    }

#define MEMORY_TRACE_ENTER1(method,param1) \
    if (isMemoryTraceEnabled()) { \
        printf("***>" method "\n",param1); \
    }

#define MEMORY_TRACE_LEAVE1(method,param1) \
    if (isMemoryTraceEnabled()) { \
        printf("***<" method "\n",param1); \
    }

#define MEMORY_TRACE_ENTER2(method,param1,param2) \
    if (isMemoryTraceEnabled()) { \
        printf("***>" method "\n",param1,param2); \
    }

#define MEMORY_TRACE_LEAVE2(method,param1,param2) \
    if (isMemoryTraceEnabled()) { \
        printf("***<" method "\n",param1,param2); \
    }

#define MEMORY_TRACE_ENTER3(method,param1,param2,param3) \
    if (isMemoryTraceEnabled()) { \
        printf("***>" method "\n",param1,param2,param3); \
    }

#define MEMORY_TRACE_LEAVE3(method,param1,param2,param3) \
    if (isMemoryTraceEnabled()) { \
        printf("***<" method "\n",param1,param2,param3); \
    }

#endif
