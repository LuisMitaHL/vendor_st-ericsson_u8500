/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   Stab ARM-NMF component class implementation
*/

#include <stab.nmf>

#include <stdio.h>
#include <string.h>
#include "OMX_Types.h"
#include "stab_lib.h"
#include "osi_trace.h"

#define ALIGN_WORD32(x) (((x) + 3) & ~0x3)

/* uncomment following define if you want to activate extensive debug traces for stab (print curves, vectors...) */
//#define __STAB_DEBUG__      
/* uncomment following define if you want to activate performance traces for stabilization */
//#define __STAB_DEBUG_PERF__

#ifdef __STAB_DEBUG_PERF__
#include "los/api/los_api.h" 
#endif

stab::stab()
{
    mNbCurves = 0;
    mCurveTimestamp = 0;
    mFramerate = 30;
    mCurveMapping.hCurvePhysical[0] = mCurveMapping.vCurvePhysical[0] = 
    mCurveMapping.hCurvePhysical[1] = mCurveMapping.vCurvePhysical[1] = 0;
    mCurveMapping.hCurveLogical[0] = mCurveMapping.vCurveLogical[0] =
    mCurveMapping.hCurveLogical[1] = mCurveMapping.vCurveLogical[1] = (t_sint32 *)NULL;
    mLastVector.vect_x = 0;
    mLastVector.vect_y = 0;
    mLastVector.timestamp = 0xFFFFFFFF;
    mStabFirstFrame = true;
    mStabEnabled = false;
    mLROnly = false;
    mCurveIdx = -1;
}

stab::~stab()
{
    stablib_destroy();
}

void METH(reportCurves)(t_uint32 hCurveAddr, t_uint32 vCurveAddr, t_uint32 timestamp) 
{
    mNbCurves++;
    MSG3("stab curves %3i: h 0x%4X v 0x%4X\n", mNbCurves, (unsigned int)hCurveAddr, (unsigned int)vCurveAddr);
    if (hCurveAddr == mCurveMapping.hCurvePhysical[0]) {
        mCurveIdx =0;
        mCurveTimestamp = timestamp;
    } else if (hCurveAddr == mCurveMapping.hCurvePhysical[1]) {
        mCurveIdx = 1;
        mCurveTimestamp = timestamp;
    } else {
        MSG0("Curve buffer not found in internal mapping!\n");
        mCurveIdx = -1;
    } 
}

void METH(activateStab)(t_bool enable, t_bool lr_only, t_uint32 framerate)
{
    MSG3("activate stab: enable=%d, lr_only=%d, framerate=%d\n", enable, lr_only, framerate);
    mStabEnabled = (bool) enable;
    mLROnly = (bool) lr_only;
    mStabFirstFrame = true;
    mFramerate = (framerate>0) ? framerate : 30; /* force 30 if variable framerate is used */
    
    /* wait for two frames for good measure                                       */
    /* in case of variable framerate, disable this check by setting a high mDelta */ 
    mDelta = (framerate>0) ? ((TIMESTAMP_TICKS/mFramerate)*2) : (TIMESTAMP_TICKS*2); 
    mMaxDelta = mDelta*2; /* panic threshold */
}

void METH(informBuffers)(t_uint32 hCurvePhysical, t_sint32 * hCurveLogical, 
                         t_uint32 vCurvePhysical, t_sint32 * vCurveLogical, 
                         t_uint32 lr_x_size, t_uint32 lr_y_size,
                         t_uint32 hr_x_size, t_uint32 hr_y_size,
                         t_uint32 lr_x_origin, t_uint32 lr_y_origin,
                         t_uint32 hr_x_origin, t_uint32 hr_y_origin,
                         t_uint32 max_lr_x_size, t_uint32 max_lr_y_size,
                         t_uint32 max_hr_x_size, t_uint32 max_hr_y_size,t_sint32 rotation)
{
    this->lr_x_size = lr_x_size;
    this->lr_y_size = lr_y_size;
    this->hr_x_size = hr_x_size;
    this->hr_y_size = hr_y_size;
    this->lr_x_origin = lr_x_origin;
    this->lr_y_origin = lr_y_origin;
    this->hr_x_origin = hr_x_origin;
    this->hr_y_origin = hr_y_origin;
    this->max_lr_x_size = max_lr_x_size;
    this->max_lr_y_size = max_lr_y_size;
    this->max_hr_x_size = max_hr_x_size;
    this->max_hr_y_size = max_hr_y_size;
    this->mRotation = rotation;
    mCurveMapping.hCurvePhysical[0] = hCurvePhysical;
    mCurveMapping.vCurvePhysical[0] = vCurvePhysical;
    mCurveMapping.hCurveLogical[0] = hCurveLogical;
    mCurveMapping.vCurveLogical[0] = vCurveLogical;
   
    mCurveMapping.hCurvePhysical[1] = hCurvePhysical+((hr_x_size*sizeof(t_sint32))&(0xFFFFFFF8UL));
    mCurveMapping.vCurvePhysical[1] = vCurvePhysical+((hr_y_size*sizeof(t_sint32))&(0xFFFFFFF8UL));

    mCurveMapping.hCurveLogical[1] = &hCurveLogical[hr_x_size];
    mCurveMapping.vCurveLogical[1] = &vCurveLogical[hr_y_size];
    

    mStabFirstFrame = true;
    stablib_destroy();
    stablib_create(hr_x_size, hr_y_size);
}

/* construct method from component.type*/
void METH(fsmInit)(fsmInit_t inits)
{
    OMX_BUFFERHEADERTYPE * dummy[2]; /* no ports are bufferSupplier without sharing port, so we never start with any buffers, so pass this dummy type to Port.init() */
    setTraceInfo(inits.traceInfoAddr, inits.id1);

    /*FIXME: handle inits.portsTunneled better (currently has no proper mapping when
      called from imgcontrolelr!) */
	mPorts[PORT_IN_LR].init(InputPort, /* Direction */
                            true,  /* bufferSupplier */
                            false, /* isHWPort */
                            &mPorts[PORT_OUT_LR], /* sharingPort */
                            dummy, /* buffers */
                            16, /* bufferCount (FIFO size ) */
                            &emptybufferdone[0], /* NMF 'return' interface */
                            PORT_IN_LR, /* portidx */
                            inits.portsDisabled & 1, /* isDisabled */
                            inits.portsTunneled & 1, /* isTunneled */
                            (Component *)this /* componentOwner */
        );

    mPorts[PORT_OUT_LR].init(OutputPort, /* Direction */
                             false,  /* bufferSupplier */
                             false, /* isHWPort */
                             &mPorts[PORT_IN_LR], /* sharingPort */
                             dummy, /* buffers */
                             16, /* bufferCount (FIFO size ) */
                             &fillbufferdone[0], /* NMF 'return' interface */
                             PORT_OUT_LR, /* portidx */
                             inits.portsDisabled & 1, /* isDisabled */
                             inits.portsTunneled & 1, /* isTunneled */
                             (Component *)this /* componentOwner */
        );

    mPorts[PORT_IN_HR].init(InputPort, /* Direction */
                            true,  /* bufferSupplier */
                            false, /* isHWPort */
                            &mPorts[PORT_OUT_HR], /* sharingPort */
                            dummy, /* buffers */
                            16, /* bufferCount (FIFO size ) */
                            &emptybufferdone[2], /* NMF 'return' interface */
                            PORT_IN_HR, /* portidx */
                            inits.portsDisabled & 4, /* isDisabled */
                            inits.portsTunneled & 4, /* isTunneled */
                            (Component *)this /* componentOwner */
        );

    mPorts[PORT_OUT_HR].init(OutputPort, /* Direction */
                             false,  /* bufferSupplier */
                             false, /* isHWPort */
                             &mPorts[PORT_IN_HR], /* sharingPort */
                             dummy, /* buffers */
                             16, /* bufferCount (FIFO size ) */
                             &fillbufferdone[2], /* NMF 'return' interface */
                             PORT_OUT_HR, /* portidx */
                             inits.portsDisabled & 4, /* isDisabled */
                             inits.portsTunneled & 4, /* isTunneled */
                             (Component *)this /* componentOwner */
        );
    mPorts[PORT_IN_BMS].init(InputPort, /* Direction */
                            true,  /* bufferSupplier */
                            false, /* isHWPort */
                            &mPorts[PORT_OUT_BMS], /* sharingPort */
                            dummy, /* buffers */
                            16, /* bufferCount (FIFO size ) */
                            &emptybufferdone[1], /* NMF 'return' interface */
                            PORT_IN_BMS, /* portidx */
                            inits.portsDisabled & 2, /* isDisabled */
                            inits.portsTunneled & 2, /* isTunneled */
                            (Component *)this /* componentOwner */
        );

    mPorts[PORT_OUT_BMS].init(OutputPort, /* Direction */
                             false,  /* bufferSupplier */
                             false, /* isHWPort */
                             &mPorts[PORT_IN_BMS], /* sharingPort */
                             dummy, /* buffers */
                             16, /* bufferCount (FIFO size ) */
                             &fillbufferdone[1], /* NMF 'return' interface */
                             PORT_OUT_BMS, /* portidx */
                             inits.portsDisabled & 2, /* isDisabled */
                             inits.portsTunneled & 2, /* isTunneled */
                             (Component *)this /* componentOwner */
        );


	init(6, /* portCount */
         mPorts, /* Ports array */
         &proxy, /* eventhandler itf */
         &me,  /* postevent itf */
         false /* invalid? */
        );

}

void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
  	if(portIdx==-1) {
		for(t_uint16 i=PORT_IN_LR;i<PORT_MAX;i++)
			 mPorts[i].setTunnelStatus ((isTunneled>>i) & 0x1);
	}
	else
		 mPorts[portIdx].setTunnelStatus ((isTunneled>>portIdx) & 0x1); 
  
   
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{

    Component::sendCommand(cmd, param);
}



/* stabber.conf emptythisbuffer interface emptyThisBuffer method, interface on input ports! */
void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 index)
{
    /* index will be 0,1,2, we want to map it to 0,2,4 (see enum stabPortID) */
    Component::deliverBuffer(index*2, buffer);
}
/* stabber.conf fillthisbuffer interface fillThisBuffer, interface on output ports! */

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 index)
{
    /* index will be 0,1,2, we want to map it to 1, 3, 5 (see enum stabPortID) */
    Component::deliverBuffer(index*2+1, buffer);
}

/* reset and process methods from component.type */
void METH(reset)()
{
}

void METH(disablePortIndication)(t_uint32 portIdx)
{
}

void METH(enablePortIndication)(t_uint32 portIdx)
{
}

void METH(flushPortIndication)(t_uint32 portIdx)
{
}

void METH(process)()
{
	/* TODO: check that we received curves... */
	OMX_BUFFERHEADERTYPE *mBuf=NULL;

    for (int i=PORT_MAX-1; i>=0; i--) {//count down to process HR before LR
        if (mPorts[i].queuedBufferCount()) {
            mBuf = mPorts[i].dequeueBuffer();
            if (mStabEnabled) {
                if ((i==PORT_IN_HR) || (mLROnly && i==PORT_IN_LR)) {
                    stabilize(mBuf);
                } else if (!mLROnly && i==PORT_IN_LR) {
                    if (applyHRVector(mBuf) == STAB_VECTOR_NOTAPPLIED) { 
                        /* wait for more recent vector */
                        mPorts[i].requeueBuffer(mBuf);
                        mBuf = NULL;
                    }
                }
            } else {
                initExtraData(mBuf, (enum e_stabPortID)i);
            }
                
            if (mBuf != NULL) {
                mPorts[i^1].returnBuffer(mBuf); // ^1 = XOR = trick to map in to out and vice versa
            }
        }
        if (mPorts[i].queuedBufferCount()) {
            /* if anything left on port, reschedule ourselves */
            Component::scheduleProcessEvent();
        }
    }
}

void stab::stabilize(OMX_BUFFERHEADERTYPE *buf)
{
    OMX_ERRORTYPE omx_error;

    t_sint16 crop_vect_x=0, crop_vect_y=0;
    OMX_SYMBIAN_DIGITALVIDEOSTABTYPE crop_vector;
     
    if (mCurveIdx==-1) return;

#ifdef __STAB_DEBUG__
    MSG0("HCurve ");
    for(int tmp=0;tmp<hr_x_size;tmp+=4)
    {
       MSG4("%ld %ld %ld %ld ",mCurveMapping.hCurveLogical[mCurveIdx][tmp],
                           mCurveMapping.hCurveLogical[mCurveIdx][tmp+1],
													 mCurveMapping.hCurveLogical[mCurveIdx][tmp+2],
													 mCurveMapping.hCurveLogical[mCurveIdx][tmp+3]);
    }
    MSG0("\n");
    MSG0("VCurve ");
    for(int tmp=0;tmp<hr_y_size;tmp+=4)
    {
       MSG4("%ld %ld %ld %ld ",mCurveMapping.vCurveLogical[mCurveIdx][tmp],
                           mCurveMapping.vCurveLogical[mCurveIdx][tmp+1],
													 mCurveMapping.vCurveLogical[mCurveIdx][tmp+2],
													 mCurveMapping.vCurveLogical[mCurveIdx][tmp+3]);
    }
    MSG0("\n");
#endif

#ifdef __STAB_DEBUG_PERF__
    t_uint32 time_before = LOS_getSystemTime();
#endif

    /* TODO:  make search & cropping window parameters dynamic.*/
if(mRotation == 0 || mRotation == 360)
    stablib_stabilize(mStabFirstFrame,
                      hr_x_size,
                      hr_y_size,
                      hr_x_origin,
                      hr_y_origin,
                      hr_x_origin,
                      hr_y_origin,
                      mFramerate,
                      mCurveMapping.hCurveLogical[mCurveIdx],
                      mCurveMapping.vCurveLogical[mCurveIdx]);
else
	stablib_stabilize(mStabFirstFrame,
                      hr_x_size,
                      hr_y_size,
                      hr_x_origin,
                      hr_y_origin,
                      hr_x_origin,
                      hr_y_origin,
                      mFramerate,
                      mCurveMapping.vCurveLogical[mCurveIdx],
                      mCurveMapping.hCurveLogical[mCurveIdx]);

#ifdef __STAB_DEBUG_PERF__
    t_uint32 time_after = LOS_getSystemTime();
    MSG1("Stabilization took %ld ms",(time_after-time_before)/1000);
#endif
                      
    mStabFirstFrame = false;
    stablib_report_croping_vectors(&crop_vect_x, &crop_vect_y);

#ifdef __STAB_DEBUG__
    MSG2("Vectors computed = %d,%d\n",crop_vect_x,crop_vect_y);
#endif

    crop_vect_x += hr_x_origin;
    crop_vect_y += hr_y_origin;

    ASSERT(crop_vect_x>=0);
    ASSERT(crop_vect_y>=0);

    crop_vector.bState = OMX_TRUE;
    crop_vector.nTopLeftCropVectorX = crop_vect_x;
    crop_vector.nTopLeftCropVectorY = crop_vect_y;
    crop_vector.nMaxOverscannedWidth = max_hr_x_size;
    crop_vector.nMaxOverscannedHeight = max_hr_y_size;
    mLastVector.vect_x = crop_vector.nTopLeftCropVectorX;
    mLastVector.vect_y = crop_vector.nTopLeftCropVectorY;
    mLastVector.timestamp = buf->nTimeStamp;

    omx_error = clearAllExtraData(buf);
    if (OMX_ErrorNone != omx_error) {
        MSG1("Error initializing extradata section : %X\n", omx_error);
    }
    else {
        OMX_VERSIONTYPE version;
        version.nVersion = OMX_VERSION;
	
        omx_error = setExtraDataField(buf, &crop_vector, version);
        if (OMX_ErrorNone != omx_error) {
            MSG1("Error writing vectors to buffer extradata section : %X\n", omx_error);
        }	
    }

}

/* add stab extradata from vector calculated on HR, only applies for LR buffers */
enum e_stabApplyVector stab::applyHRVector(OMX_BUFFERHEADERTYPE *buf)
{
    OMX_ERRORTYPE omx_error;
/*TODO: The timestamp based logic needs to be corrected*/
#if 0
    if (buf->nTimeStamp-mLastVector.timestamp > mMaxDelta) {
        /* we had a problem and this LR buffer has waited too long for a vector. 
        TODO: determine error handling */
        omx_error = clearAllExtraData(buf);
        if (OMX_ErrorNone != omx_error) {
            MSG1("Error initializing extradata section : %X\n", omx_error);
        }
        else {
            OMX_SYMBIAN_DIGITALVIDEOSTABTYPE vector;
            OMX_VERSIONTYPE version;
            version.nVersion = OMX_VERSION;
            vector.bState = OMX_TRUE;
            vector.nTopLeftCropVectorX = lr_x_origin;
            vector.nTopLeftCropVectorY = lr_y_origin;
            vector.nMaxOverscannedWidth = max_lr_x_size;
            vector.nMaxOverscannedHeight = max_lr_y_size;


            omx_error = setExtraDataField(buf, &vector, version);
            if (omx_error != OMX_ErrorNone) {
                MSG1("Error writing vectors to LR buffer extradata section: %X\n", omx_error);
            }
        }
        return STAB_VECTOR_APPLIED;
    }

    if (buf->nTimeStamp-mLastVector.timestamp > mDelta) {
        /* last vector is a bit too old, we should be getting an 
           HR soon, wait for that. */
        return STAB_VECTOR_NOTAPPLIED;
    }
#endif
   
	 /* resize vector for LR */
    omx_error = clearAllExtraData(buf);
    if (OMX_ErrorNone != omx_error) {
        MSG1("Error initializing extradata section : %X\n", omx_error);
    } else {
        OMX_SYMBIAN_DIGITALVIDEOSTABTYPE vector;
        OMX_VERSIONTYPE version;
        version.nVersion = OMX_VERSION;
        vector.bState = OMX_TRUE;
        vector.nTopLeftCropVectorX = (mLastVector.vect_x*lr_x_size)/hr_x_size;
        vector.nTopLeftCropVectorY = (mLastVector.vect_y*lr_y_size)/hr_y_size;
        vector.nMaxOverscannedWidth = max_lr_x_size;
        vector.nMaxOverscannedHeight = max_lr_y_size;

        //printf("vector LR (from HR): x=%d, y=%d\n", lr_vect_x, lr_vect_y);
        omx_error = setExtraDataField(buf, &vector, version);
        if (omx_error != OMX_ErrorNone) {
            MSG1("Error writing vectors to LR buffer extradata section: %X\n", omx_error);
        }
    }
    return STAB_VECTOR_APPLIED;
}

/* Clear all extradata and add first extradata structures. This supposes that we're the first 
   in the flow to add extradata */ 
OMX_ERRORTYPE stab::initExtraData(OMX_BUFFERHEADERTYPE* pOmxBufHdr, enum e_stabPortID port_id)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;

    if ((PORT_IN_HR != port_id) && (PORT_IN_BMS != port_id) && (PORT_IN_LR != port_id)) {
        return OMX_ErrorNone;
    }

    error = clearAllExtraData(pOmxBufHdr);
    if (OMX_ErrorNone != error) return error;

    switch (port_id) {
        case PORT_IN_HR: /* fall through */
        case PORT_IN_LR:
            OMX_SYMBIAN_DIGITALVIDEOSTABTYPE stab_extradata;
            OMX_VERSIONTYPE version;
            
            stab_extradata.bState = OMX_FALSE;
            stab_extradata.nTopLeftCropVectorX=0; 
            stab_extradata.nTopLeftCropVectorY=0;
            stab_extradata.nMaxOverscannedWidth=0;
            stab_extradata.nMaxOverscannedHeight=0;
            version.nVersion=OMX_VERSION;

            error = setExtraDataField(pOmxBufHdr, &stab_extradata, version);
            break;
        case PORT_IN_BMS:
            break;
        default: break;
    }
    return error;
}

OMX_ERRORTYPE stab::clearAllExtraData(OMX_BUFFERHEADERTYPE* pOmxBufHdr)
{
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no extradata */
		return OMX_ErrorNone;
	}
	
	/* get extradata start pointer */
	OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)ALIGN_WORD32((OMX_U32)pCur);
	
	/* check that there is enough space for an extradata header */
	if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen < (OMX_U8*)pCurExtra + ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE)))
	{
		return OMX_ErrorInsufficientResources;
	}

    /* add extradata terminator */
    pCurExtra->nSize = ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE));
    pCurExtra->nVersion.nVersion = 0;
    pCurExtra->nPortIndex = 0;
    pCurExtra->eType = OMX_ExtraDataNone;
    pCurExtra->nDataSize = 0;
		
	return OMX_ErrorNone;
}


OMX_ERRORTYPE stab::setExtraDataField(OMX_U32 nExtradataType, OMX_BUFFERHEADERTYPE* pOmxBufHdr,
						void* pExtradataData, OMX_U32 nExtradataDataSize, OMX_VERSIONTYPE version)
{
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no need to add extradata */
		return OMX_ErrorNone;
	}
	
	/* get extradata start pointer */
	OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)ALIGN_WORD32((OMX_U32)pCur);
	
	/* check that there is enough space for an extradata header */
	if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen < (OMX_U8*)pCurExtra 
        + ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof (OMX_U8) + nExtradataDataSize))
	{
		return OMX_ErrorInsufficientResources;
	}
	
	/* search if field already exists */
	while (OMX_ExtraDataNone != pCurExtra->eType)
	{
		/* we have found our extradata field ! */
		if ((OMX_EXTRADATATYPE)nExtradataType == pCurExtra->eType)
		{
			break;
		}
		/* check that there is enough space for another extradata header */
		else if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen > (OMX_U8*)pCurExtra + pCurExtra->nSize
				+ ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + nExtradataDataSize))
		{
			pCurExtra = (OMX_OTHER_EXTRADATATYPE*)((OMX_U8*)pCurExtra + pCurExtra->nSize);
		}
		else
		{
			return OMX_ErrorInsufficientResources;
		}
	}
	
	/* we have found our extradata field */
	if (OMX_ExtraDataNone != pCurExtra->eType)	
	{
		/* check that there is enough space for datas */
		if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen < (OMX_U8*)pCurExtra
				+ ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + nExtradataDataSize))
		{
			return OMX_ErrorInsufficientResources;
		}	
		
		/* set flag */
		pOmxBufHdr->nFlags |= OMX_BUFFERFLAG_EXTRADATA;
			
		memcpy ((void*)pCurExtra->data, pExtradataData, nExtradataDataSize);
	}
	/* we must add our extradata field */
	else
	{
		/* check that there is enough space for 2 headers and our datas */
		if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen < (OMX_U8*)pCurExtra
			+ ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + nExtradataDataSize)
			+ ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE)))
		{
			return OMX_ErrorInsufficientResources;
		}
		
		/* set flag */
		pOmxBufHdr->nFlags |= OMX_BUFFERFLAG_EXTRADATA;
		
		/* fill header */
		pCurExtra->nSize = ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + nExtradataDataSize);
        pCurExtra->nVersion = version;
        pCurExtra->nPortIndex = pOmxBufHdr->nOutputPortIndex;
        pCurExtra->eType = (OMX_EXTRADATATYPE)nExtradataType;
        pCurExtra->nDataSize = nExtradataDataSize;
        memcpy ((void*)pCurExtra->data, pExtradataData, nExtradataDataSize);

        /* add extradata terminator */
        pCurExtra = (OMX_OTHER_EXTRADATATYPE*)((OMX_U8*)pCurExtra + pCurExtra->nSize);
        pCurExtra->nSize = ALIGN_WORD32(sizeof(OMX_OTHER_EXTRADATATYPE));
        pCurExtra->nVersion.nVersion = OMX_VERSION;
        pCurExtra->nPortIndex = 0;
        pCurExtra->eType = OMX_ExtraDataNone;
        pCurExtra->nDataSize = 0;
	}
	return OMX_ErrorNone;
}


OMX_ERRORTYPE stab::setExtraDataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* pExtradataData, OMX_VERSIONTYPE version)
{
	return setExtraDataField(OMX_SYMBIAN_ExtraDataVideoStabilization, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE), version);
}
