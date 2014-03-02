/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_grabctl.h"

#undef   OMXCOMPONENT
#define  OMXCOMPONENT "EXTCAM_GRABCTL"
#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#include "debug_trace.h"

void CGrabControl::setConfigureControl(
        Iext_grabctl_api_configure configureControl)
{
    mConfigureControl = configureControl;
}

void CGrabControl::setStabConfigure(
        Iext_stab_api_configure stabConfigure)
{
    mStabConfigure = stabConfigure;
}

void CGrabControl::setForceDequeueBuffer(
        t_uint16 port_idx)
{
    /* In case of SIA lock, we need to dequeue buffer located at MPC side. */
    DBGT_PTRACE("ForceDequeueBuffer on port %d", port_idx);
    mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,0);
}

void CGrabControl::setEndCapture(
        t_uint16 port_idx)
{
    DBGT_PTRACE("EndCapture on port %d", port_idx);
    mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,2);
}

void CGrabControl::setEndCaptureNow(
        t_uint16 port_idx)
{
    DBGT_PTRACE("EndCaptureNow on port %d", port_idx);
    mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,3);
}

void CGrabControl::setStartCapture(
        t_uint16 port_idx)
{
    DBGT_PTRACE("StartCapture on port %d", port_idx);
    mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,4);
}

void CGrabControl::setStartOneShotCapture(
        t_uint16 port_idx)
{
    DBGT_PTRACE("StartOneShotCapture on port %d", port_idx);
    mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,5);
}

void CGrabControl::startBurstCapture(
        t_uint16 port_idx,
        t_uint16 nbBurst)
{
    DBGT_PTRACE("StartBurstCapture on port %d", port_idx);
    mConfigureControl.setConfig((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,6+nbBurst);
}

void CGrabControl::callInterface(
        t_grab_desc *pGrabDesc,
        struct s_grabParams grabparams)
{
    mConfigureControl.setParams(grabparams,(e_grabctlPortID)pGrabDesc->port_idx,pGrabDesc->bufferCount);
    mStabConfigure.setParams(grabparams, (e_grabctlPortID)pGrabDesc->port_idx);
}

void CGrabControl::configureHook(
        struct s_grabParams &grabParams,
        t_uint16 port_idx)
{
    grabParams.enable_stab = 0;
    grabParams.frameSkip   = 0;

    if (port_idx==0)
        grabParams.frameSkip= mVfFrameSkip;
    else if (port_idx==1)
        grabParams.frameSkip= mSsFrameSkip;
}

void CGrabControl::setFrameSkipValues(
        t_uint8 vfFrameSkip,
        t_uint8 ssFrameSkip)
{
    mVfFrameSkip = vfFrameSkip;
    mSsFrameSkip = ssFrameSkip;
}

OMX_ERRORTYPE CGrabControl::activateStab(ifmport *port)
{
    //Stab not supported today
    return OMX_ErrorNone;
}

void CGrabControl::deactivateStab()
{
    //Stab not supported today
}

OMX_ERRORTYPE CGrabControl::allocateStabBuffers(ifmport *port)
{
    //Stab not supported today
    return OMX_ErrorNone;
}

OMX_ERRORTYPE CGrabControl::freeStabBuffers()
{
    //Stab not supported today
    return OMX_ErrorNone;
}

void CGrabControl::getOverscanResolution(int xsize, int ysize, e_grabFormat grabformat, float zoomfactor, int *xoversize, int *yoversize)
{
    if (!mStabEnabled) {
        *xoversize = xsize;
        *yoversize = ysize;
        return;
    }
    int nom, den;
    /* zoomfactor should influence nominator and denominator someday, currently fixed at 130% because
       on ED and early V1 size is static */
    nom = 13;
    den = 10;

    /* size = (size * 130%) rounded down to 16 (MB size) */
    *xoversize = ((xsize*nom)/den)&(~7);
    *yoversize = ((ysize*nom)/den)&(~7);

    if (grabformat == GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED) {
        /* sizes must be right multiples of 32 , round down. */
        *xoversize = (*xoversize) & (~0x1F);
        *yoversize = (*yoversize) & (~0x1F);
    }
}
