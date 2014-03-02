/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_HSMCAM_H_
#define _EXT_HSMCAM_H_

#include "ext_hsm.h"
#include "ext_opmodemgr.h"
#include "ext_grabctl.h"
#include "ext_grab.h"
#include "ext_capture_context.h"
#include "ext_camera_ctrllib.h"

/* timer interface */
#include "timer/timerWrapped.hpp"

/*Defines*/
#define HW_DPHY_ENABLE	1
#define HW_DPHY_DISABLE	0

/* +ER363747 */
// define several type of hardware error
#define CAMERA_DSP_PANIC 0
#define CAMERA_BOOT_FAILED 1
/* +ER363747 */

class CAM_SM : public COM_SM {

public:
    CAM_SM(ENS_Component &enscomp): COM_SM(enscomp) {}

    void watchdogStop(
            void);
    void watchdogStart(
            void);
protected:

    virtual SCF_STATE ProcessPendingEvents(
            s_scf_event const *e);

    /* ------------------ */
    /* Boot state machine */
    /* ------------------ */
    virtual SCF_STATE Booting(
            s_scf_event const *e);
    SCF_STATE InitingYUVCameraModule(
            s_scf_event const *e);

    /* ------------------ */
    /* Operating modes    */
    /* ------------------ */
    virtual SCF_STATE EnteringPreview(
            s_scf_event const *e);
    virtual SCF_STATE PreparingGrabConfig(
            s_scf_event const *e);
    SCF_STATE EnteringPreview_DoingPreviewConfiguration(
            s_scf_event const *e);

    SCF_STATE ExitingPreview(
            s_scf_event const *e);
    SCF_STATE ExitingPreview_StoppingVpip(
            s_scf_event const *e);

    SCF_STATE BackPreview(
            s_scf_event const *e);
    SCF_STATE BackPreview_StartingVpip(
            s_scf_event const *e);
    SCF_STATE BackPreview_AutoPause(
            s_scf_event const *e);

    SCF_STATE EnteringCapture(
            s_scf_event const *e);
    SCF_STATE EnteringCapture_DoingCaptureConfiguration(
            s_scf_event const *e);
    SCF_STATE EnteringCapture_SendBuffers(
            s_scf_event const *e);
    SCF_STATE EnteringCapture_FillBufferDone(
            s_scf_event const *e);

    SCF_STATE ExitingCapture(
            s_scf_event const *e);
    SCF_STATE ExitingCapture_StoppingVpip(
            s_scf_event const *e);
    SCF_STATE ExitingCapture_CancelCaptureConfiguration(
            s_scf_event const *e);

    /* ------------------ */
    /* Enabling Ports     */
    /* ------------------ */
    virtual SCF_STATE	EnablePortInIdleExecutingState(
            s_scf_event const *e);
    SCF_STATE EnablePortInIdleExecutingState_StopVpip(
            s_scf_event const *e);
    SCF_STATE EnablePortInIdleExecutingState_ConfigurePipe(
            s_scf_event const *e);
    SCF_STATE EnablePortInIdleExecutingState_CfgGrabStartVpip(
            s_scf_event const *e);
    SCF_STATE EnablePortInIdleExecutingState_SendOmxCb(
            s_scf_event const *e);

    virtual void timeoutAction(
            void);
public :
    /* Operating mode related */
    t_uint16                    CapturePort;
    CopModeMgr                 *pOpModeMgr;
    CGrabControl               *pGrabControl;
    CGrab                      *pGrab;
    CExtIspctlComponentManager *pExtIspctlComponentManager;
    enumExtCameraSlot           HSMExtCamSlot;
    CCapture_context           *pCapture_context;

    t_uint8 enabledPort;

    unsigned int watchdogIntervalMsec;
    bool isWatchdogTimerRunning;
    Itimer_api_timer mTimer;
    unsigned int watchdogCounter;
    bool flushRequest;
};

#endif /* _EXT_HSMCAM_H_ */

