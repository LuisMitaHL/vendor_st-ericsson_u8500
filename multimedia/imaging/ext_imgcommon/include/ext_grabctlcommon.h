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

#include "ext_grabctl_types.idt.h"
#include "ext_grab_types.idt.h"

//#include "cm/inc/cm.hpp" /* Jean hack to allocate buffers in ESRAM. Need to move this to HwBuffer instead */

typedef struct {
	OMX_COLOR_FORMATTYPE grabFormat;
	t_uint32 port_idx;
	t_uint32 xframeSize;
	t_uint32 yframeSize;
	t_uint32 destBufferAddr;
	t_uint32 disableGrabCache;
	t_uint32 cacheBufferAddr;
	t_uint32 bufferCount;
    t_uint32 xwindowSize;
    t_uint32 ywindowSize;
    t_uint32 xwindowOffset;
    t_uint32 ywindowOffset;
	t_uint32 bufferSize;
	OMX_IMAGE_CODINGTYPE imageCompressionFormat;
}t_grab_desc;



#define NB_PIPE_ID 4



#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CGrabControlCommon);
#endif
class CGrabControlCommon {

public:
    CGrabControlCommon(ENS_Component &enscomp);
    ~CGrabControlCommon();


	void configureGrabResolutionFormat(ifmport*);
	void configure( t_grab_desc * pGrabDesc);
    virtual void configureHook(struct s_grabParams &grabParams,
            t_uint16 port_idx) {};


	virtual void setEndCapture(t_uint16)=0;
	virtual void setStartCapture(t_uint16)=0;
	virtual void setStartOneShotCapture(t_uint16)=0;


	virtual void callInterface(t_grab_desc *pGrabDesc,
            struct s_grabParams grabparams) = 0;

    OMX_ERRORTYPE freeGrabCache();

    e_grabFormat gconvertFormatForFW(OMX_COLOR_FORMATTYPE omxformat);
    char* grabFormatToString(e_grabFormat grab_format);

protected:
    t_cm_memory_handle mGrabCacheHandle;
    t_uint32 mCacheAddrPhys;
           ENS_Component&  mENSComponent;
};

#endif


