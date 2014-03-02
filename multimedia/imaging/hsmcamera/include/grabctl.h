/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef GRABCTL_H_
#define GRABCTL_H_

#include "host/grabctl/api/configure.hpp"
#include "stab/api/configure.hpp"
#include "grabctlcommon.h"
#include "camport.h"
#include "opmodemgr.h"
#include "mmhwbuffer.h"
#include "cm/inc/cm.hpp" /* Jean hack to allocate buffers in ESRAM. Need to move this to HwBuffer instead */
#include "omxcamera.h"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CGrabControl);
#endif

typedef enum GrabState {
    GRAB_STATE_UNCONFIGURED = 0,
    GRAB_STATE_ENDCAPTURE,
    GRAB_STATE_ENDCAPTURENOW,
    GRAB_STATE_STARTCAPTURE,
    GRAB_STATE_STARTONESHOTCAPTURE,
    GRAB_STATE_STARTBURSTCAPTURE,
    GRAB_STATE_STARTHIDDENCAPTURE,
    GRAB_STATE_INVALID
} GrabState;

class CGrabControl : public CGrabControlCommon {
public:

~CGrabControl();
CGrabControl(ENS_Component &mENSComponent, CopModeMgr *pOpModeMgr):
    CGrabControlCommon(mENSComponent),
    mStabEnabled(false),
    mEsramSuspended(false),
    mWaitingForEsram(false),
    pOMXCam((COmxCamera*)&mENSComponent),
    mpOpModeMgr(pOpModeMgr),
    stab_buf_allocated(false),
    mPortVF(NULL),
    mPortRecord(NULL) {
        stab_buf_h = (t_cm_memory_handle)(NULL);
        for (int i = 0; i < CAMERA_NB_OUT_PORTS; i++)
            grabPortsState[i] = GRAB_STATE_UNCONFIGURED;
    };

	virtual void setEndCapture(t_uint16);
	void setEndCaptureNow(t_uint16);
	virtual void setStartCapture(t_uint16);
	virtual void setStartOneShotCapture(t_uint16);
	void startBurstCapture(t_uint16 port_idx, t_uint16 nbBurst);
	void setStartHiddenCapture(t_uint16 port_idx, t_uint16 nbBefore);
	void setStartHiddenBMS(t_uint16 hidden_bms, t_uint16 nbBefore);
	void setBMLCapture(t_uint16 hidden_bms);
	void setHiddenBMSBuffers(t_uint32* bufferadd, t_uint32 noOfBuffers ,t_uint16 xsize, t_uint16 ysize, t_uint16 linelen, t_uint16 pack);
	virtual void callInterface(t_grab_desc *pGrabDesc, struct s_grabParams grabparams);

	void setConfigureControl(Igrabctl_api_configure configureControl);

    void setStabConfigure(Istab_api_configure stabConfigure);


    virtual void configureHook(struct s_grabParams &grabParams, t_uint16 port_idx);


    OMX_ERRORTYPE configureStab(bool resSusp, t_sint32 rotation);
    void deactivateStab();
    OMX_BOOL isStabActivated() {return (OMX_BOOL)stab_buf_allocated;}
    OMX_BOOL IsStabEnabled();

    void getOverscanResolution(int xsize, int ysize, e_grabFormat grabformat, float zoomfactor, int& xoversize, int&yoversize, t_uint16 port_idx);
    void rectifyOverscanResolution(int xsize, int ysize,int &xoversize, int &yoversize);

    OMX_ERRORTYPE freeStabBuffers(); /* public because it needs to be done before destructor */
    void setEndCaptureforHiddenBMS();

    /** Returns current grab mode as string
     *  @param camport port for which status is required
     */
    const char* stringfyGrabStatus(const uint32_t camport);

    bool mStabEnabled;
    bool mEsramSuspended;
    bool mWaitingForEsram;

private:

    COmxCamera* pOMXCam;
	Igrabctl_api_configure mConfigureControl;

    Istab_api_configure mStabConfigure;
    CopModeMgr *mpOpModeMgr;
    bool stab_buf_allocated;

#ifdef HCURVE_IN_ESRAM
//for HCurve buffer in ESRAM start
    t_cm_memory_handle stab_buf_h;
//for HCurve buffer in ESRAM end
#else
	//for HCurve buffer in DDR start
	MMHwBuffer * stab_buf_h;
	//for HCurve buffer in DDR end
#endif

	MMHwBuffer * stab_buf_v;
	camport *mPortVF, *mPortRecord;
    OMX_ERRORTYPE allocateStabBuffers(camport *port, t_uint32& proj_v_buf_phys, t_sint32 *&proj_v_buf_log, t_uint32& proj_h_buf_phys, t_sint32 *&proj_h_buf_log,t_sint32 rotation);
    GrabState grabPortsState[CAMERA_NB_OUT_PORTS];
};

#endif /* GRABCTL_H_ */
