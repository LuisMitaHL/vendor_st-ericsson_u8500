/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _GRABCONTROLCOMMON_H_
#define _GRABCONTROLCOMMON_H_

#include "OMX_Index.h"
#include "OMX_Core.h"
#include "ENS_Component.h"
#include "ifmport.h"
#include "grabctl_types.idt.h"
#include "grab_types.idt.h"

typedef struct {
	OMX_COLOR_FORMATTYPE grabFormat;
	t_uint32 port_idx;
	t_uint32 xframeSize;
	t_uint32 yframeSize;
	t_uint32 destBufferAddr;
	t_uint32 disableGrabCache;
	t_uint32 cacheBufferAddr;
	t_uint32 bufferCount;
	t_sint32 rotation;
	s_grabBMLParams bmlDesc;
    t_uint32 xwindowSize;
	t_uint32 ywindowSize;
    t_uint32 xwindowOffset;
    t_uint32 ywindowOffset;
    t_uint8  keepRatio;
}t_grab_desc;



#define NB_PIPE_ID 4



#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CGrabControlCommon);
#endif
class CGrabControlCommon {

public:
CGrabControlCommon(ENS_Component &enscomp):
    mCacheAddrPhys(0),
    mENSComponent(enscomp) {};

~CGrabControlCommon();

	void configureGrabResolutionFormat(ifmport*, t_uint8 keepRatio, t_sint32 rotation=0,t_uint32 stripeCount=0, t_uint32 stripeNumber=0);
	void configureMemGrabResolutionFormat(ifmport*, t_uint32 embeddedLinesNb,t_uint32 interlineSize, e_BMLPixelOrder pixelOrder, t_uint32 stripeCount=0, t_uint32 stripeNumber=0, t_uint32 stripeSize=0);
	void configure( t_grab_desc * pGrabDesc, t_bool isBML);
    virtual void configureHook(struct s_grabParams &grabParams, t_uint16 port_idx) {};


	virtual void setEndCapture(t_uint16)=0;
	virtual void setStartCapture(t_uint16)=0;
	virtual void setStartOneShotCapture(t_uint16)=0;


	virtual void callInterface(t_grab_desc *pGrabDesc, struct s_grabParams grabparams) = 0;

	OMX_ERRORTYPE allocateGrabCache(t_uint16 xsize);
    OMX_ERRORTYPE freeGrabCache();

    t_uint16 gconvertRotationForFW(t_sint32 rotation);
    e_grabFormat gconvertFormatForFW(OMX_COLOR_FORMATTYPE omxformat, t_bool isBML);
    const char* grabFormatToString(e_grabFormat grab_format);
    t_uint32 mCacheAddrPhys;
protected:
    /* eSRAM is shared between hsmcamera and imgcommon
     * and can not used by hsmcamera and imgcommon
     * simultaneously,
     * therefore handle to it is made static.
     * It should be cleared after use by hsmcamera and
     * imgcommon. */
    static t_cm_memory_handle mGrabCacheHandle;
    ENS_Component&  mENSComponent;
};

#endif


