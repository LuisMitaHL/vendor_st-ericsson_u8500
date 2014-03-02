/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMG_ENS_TRACE_H_
#define _IMG_ENS_TRACE_H_

#include <omxil/OMX_Component.h>
#include <inc/type.h>

typedef struct
{
	unsigned long  parentHandle;
	unsigned short traceEnable;
	unsigned long  dspAddr;
} ImgEns_TraceInfo;

class IMGENS_API Img_TraceObject
{
public:
	Img_TraceObject(): mId1(0), mTraceInfoPtr(0) {};

	void setTraceInfo(ImgEns_TraceInfo * ptr, unsigned int val) {mTraceInfoPtr = ptr; mId1 = val;}
	ImgEns_TraceInfo * getTraceInfoPtr() const { return mTraceInfoPtr; }
	unsigned int getId1() const { return mId1; }
	unsigned int getParentHandle() const { /*if (mTraceInfoPtr) return mTraceInfoPtr->parentHandle; else*/return 0;}
	unsigned short getTraceEnable() const { /*if (mTraceInfoPtr) return mTraceInfoPtr->traceEnable; else*/return 0;}
	unsigned int getDspAddr() const { /*if (mTraceInfoPtr) return mTraceInfoPtr->dspAddr; else*/return 1;}
private:
	unsigned int mId1;
	ImgEns_TraceInfo * mTraceInfoPtr;
};

typedef struct
{
	t_uint16 parentHandleH;
	t_uint16 parentHandleL;
	t_uint16 traceEnable;
} ImgEns_TraceInfoDspCreate;


class Img_TraceBuilder
{
public:
	Img_TraceBuilder() {mSharedTraceInfoPtr=NULL; }
	virtual ~Img_TraceBuilder() { if (mSharedTraceInfoPtr) delete mSharedTraceInfoPtr; }

	ImgEns_TraceInfo * getSharedTraceInfoPtr(void) {return mSharedTraceInfoPtr; };

	virtual void  setTraceZone(OMX_U32 handle, OMX_U16 traceLevel);
	OMX_ERRORTYPE createInitialZone();
	virtual OMX_ERRORTYPE createExtraZone(t_uint32 domainId) = 0;
	virtual OMX_ERRORTYPE deleteExtraZone() = 0;

protected:
	ImgEns_TraceInfo * mSharedTraceInfoPtr;
};

class ImgEns_TraceBuilderHost: public Img_TraceBuilder 
{
public:
	virtual void setTraceZone(OMX_U32 handle, OMX_U16 traceLevel);
	virtual OMX_ERRORTYPE createExtraZone(OMX_U32 domainId);
	virtual OMX_ERRORTYPE deleteExtraZone();
};


extern "C" 
{
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

#endif // _IMG_ENS_TRACE_H_
