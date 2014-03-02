/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TraceObject.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _TRACEOBJECT_H_
#define _TRACEOBJECT_H_

#include "ENS_Redefine_Class.h"

#include <inc/type.h>
#include "host/trace.idt.h"


class TraceObject {
    public:
        TraceObject(): mId1(0), mTraceInfoPtr(0)  {};

		void setTraceInfo(TraceInfo_t * ptr, unsigned int val) { mTraceInfoPtr = ptr; mId1 = val; return; };
 		TraceInfo_t * getTraceInfoPtr() const { return mTraceInfoPtr; };
		unsigned int getId1() const { return mId1; };
		unsigned int getParentHandle() const { if (mTraceInfoPtr) return mTraceInfoPtr->parentHandle; else return 0; };
		unsigned short getTraceEnable() const { if (mTraceInfoPtr) return mTraceInfoPtr->traceEnable; else return 0;};
		unsigned int getDspAddr() const { if (mTraceInfoPtr) return mTraceInfoPtr->dspAddr; else return 1; };

	private:
        unsigned int  mId1;
		TraceInfo_t * mTraceInfoPtr;
 };

#endif // _TRACEOBJECT_H_
