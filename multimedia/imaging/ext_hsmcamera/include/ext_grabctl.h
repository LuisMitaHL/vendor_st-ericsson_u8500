/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_GRABCTL_H_
#define _EXT_GRABCTL_H_

#include "host/ext_grabctl/api/configure.hpp"
#include "ext_stab/api/configure.hpp"
#include "ext_grabctlcommon.h"
#include "ext_camport.h"
#include "mmhwbuffer.h"
#include "cm/inc/cm.hpp" /* Jean hack to allocate buffers in ESRAM. Need to move this to HwBuffer instead */

class CGrabControl : public CGrabControlCommon {
public:
    CGrabControl(ENS_Component &mENSComponent):
        CGrabControlCommon(mENSComponent),
        mStabEnabled(false),
        stab_buf_allocated(false) {};

    void setForceDequeueBuffer(
            t_uint16);

    virtual void setEndCapture(
            t_uint16);

    void setEndCaptureNow(
            t_uint16);

    virtual void setStartCapture(
            t_uint16);

    virtual void setStartOneShotCapture(
            t_uint16);

    void startBurstCapture(
            t_uint16 port_idx,
            t_uint16 nbBurst);

    virtual void callInterface(
            t_grab_desc *pGrabDesc,
            struct s_grabParams grabparams);

    void setConfigureControl(
            Iext_grabctl_api_configure configureControl);

    void setStabConfigure(
            Iext_stab_api_configure stabConfigure);

    void setFrameSkipValues(
            t_uint8 vfFrameSkip,
            t_uint8 ssFrameSkip);

    virtual void configureHook(
            struct s_grabParams &grabParams,
            t_uint16 port_idx);

    OMX_ERRORTYPE activateStab(ifmport *port);

    void deactivateStab();

    OMX_BOOL isStabActivated()
    {
        return (OMX_BOOL)stab_buf_allocated;
    }

    void getOverscanResolution(
            int xsize,
            int ysize,
            e_grabFormat grabformat,
            float zoomfactor,
            int *xoversize,
            int *yoversize);

    bool mStabEnabled;

private:
    Iext_grabctl_api_configure mConfigureControl;

    Iext_stab_api_configure mStabConfigure;

    bool stab_buf_allocated;
    t_cm_memory_handle stab_buf_h;
    //OMX_U32 stab_buf_v;
    MMHwBuffer * stab_buf_v;

    OMX_ERRORTYPE allocateStabBuffers(ifmport *port);
    OMX_ERRORTYPE freeStabBuffers();

    t_uint8 mVfFrameSkip;
    t_uint8 mSsFrameSkip;
};

#endif /* _EXT_GRABCTL_H_ */
