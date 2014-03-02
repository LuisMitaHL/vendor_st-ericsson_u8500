/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_CAMERA_H_
#define _EXT_CAMERA_H_

#include "ENS_DBC.h"
#include "OMX_Index.h"
#undef LOG_TAG
#include "IFM_HostNmfProcessingComp.h"

#include "ext_ispctl_component_manager.h"

#include "ext_sia_platform_manager.h"
#include "ext_grabctl.h"
#include "ext_grab.h"

#include "ext_img_network.hpp"
#include "ext_grabctl/api/acknowledges.hpp"
#include "ext_grabctl/api/error.hpp"
#include "ext_grab/api/alert.hpp"
#include "timer/timerWrapped.hpp"

#include "ext_deferredeventmgr.h"
#include "ext_opmodemgr.h"

#include "ext_hostnmf_cb.h"
#include "ext_hsmcam.h"

#include "IFM_Index.h"
#include "IFM_Types.h"

#include "flash_api.h"

#undef   LOG_TAG
#define  LOG_TAG DBGT_TAG
#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#undef   DBGT_PREFIX
#define  DBGT_PREFIX "ECAM"
#include "debug_trace.h"

class Camera : public IFM_HostNmfProcessingComp,
    public ext_grabctl_api_acknowledgesDescriptor,
    public ext_grabctl_api_errorDescriptor,
    public ext_grab_api_alertDescriptor,
    public timer_api_alarmDescriptor
{

public:

    Camera(ENS_Component &enscomp, enumExtCameraSlot cam=ePrimaryExtCamera);
    virtual ~Camera();
    //HSMCAM hsm;
    CAM_SM * p_cam_sm;
    virtual OMX_ERRORTYPE applyConfig(
            OMX_INDEXTYPE nIndex,
            OMX_PTR pStructure
            );

    virtual OMX_ERRORTYPE retrieveConfig(
            OMX_INDEXTYPE nConfigIndex,
            OMX_PTR pComponentConfigStructure
            );

    virtual OMX_ERRORTYPE instantiate();
    virtual OMX_ERRORTYPE start() ;
    virtual OMX_ERRORTYPE stop() ;

    virtual OMX_ERRORTYPE configure();

    virtual OMX_ERRORTYPE deInstantiate() ;

    virtual OMX_ERRORTYPE emptyThisBuffer(
            OMX_BUFFERHEADERTYPE* pBuffer);

    virtual OMX_ERRORTYPE doSpecificSendCommand(
            OMX_COMMANDTYPE cmd,
            OMX_U32 nParam,
            OMX_BOOL &bDeferredCmd) ;

    virtual OMX_ERRORTYPE doSpecificEventHandler_cb(
            OMX_EVENTTYPE event,
            OMX_U32 nData1,
            OMX_U32 nData2,
            OMX_BOOL &bDeferredEventHandler) ;

    OMX_ERRORTYPE setOperatingMode(
            t_operating_mode_camera aOpMode);

    CopModeMgr iOpModeMgr;

    OMX_BOOL iPanic;

    /* +ER363747 */
    OMX_BOOL instantiateDone;
    OMX_BOOL startDone;
    /* -ER363747 */

    /* ARM-NMF fillBufferDone callback */
    virtual void fillBufferDone(
            OMX_BUFFERHEADERTYPE * pBuffer);
    virtual OMX_ERRORTYPE fillThisBuffer(
            OMX_BUFFERHEADERTYPE* pBuffer);

    void errorRecovery();

    /* alarm callback */
    void alarmCallback();

    /* timer interface */
    Itimer_api_timer mTimer;

    void autoFocusThread();

    Camera_Ctrllib* cam_Ctrllib;

    CamCtrl_DriverData_t* cam_h;
    int m_auto_flash_on;
    int m_is_need_flash_on;
    t_uint32 auto_flash_on_threshold;
    t_uint32 auto_flash_off_threshold;
    int synchro_flash_support;

    void printPrivateContext(OMX_OTHER_PARAM_PRIVATE_CONTEXT* pCtx);

protected :
    pthread_mutex_t mutex;
    int is_af_cancel;
    pthread_t af_thread_t;
    OMX_IMAGE_FLASHCONTROLTYPE  m_flash_control;
    void fillBufferDoneVPB1(
            OMX_BUFFERHEADERTYPE *pOMXBuffer);
    void fillBufferDoneVPB0(
            OMX_BUFFERHEADERTYPE *pOMXBuffer);

    /* grabctl_api_acknowledges callbacks */
    virtual void configured(
            t_uint16 port_idx);
    virtual void clockrunning(
            t_uint16 port_idx);
    virtual void clockwaitingforstart(
            t_uint16 port_idx);
    /*grabctl_api_error callbacks */
    virtual void error(
            enum e_grabError error_id,
            t_uint16 data,
            enum e_grabPipeID pipe_id) ;
    /* grab_api_alert callbacks */
    virtual void info(
            enum e_grabInfo info_id,
            t_uint16 buffer_id,
            t_uint32 timestamp);
    virtual void error(
            enum e_grabError error_id,
            t_uint16 data,
            t_uint16 buffer_id,
            t_uint32 timestamp);
    virtual void debug(
            enum e_grabDebug debug_id,
            t_uint16 buffer_id,
            t_uint16 data1,
            t_uint16 data2,
            t_uint32 timestamp);
    /* timer_api_alarm callback */
    virtual void signal();

    ext_img_network * mpImgNetwork;
    timerWrapped* pTimer;
    Iext_grabctl_api_configure mIgrabctlConfigure;
    Iext_stab_api_configure mIstabConfigure;

private :

    void constructSM(
            ENS_Component &enscomp);
    void destroySM();
    int  configureIPP(
            int enable);
    void cameraFlashOnOff( int enable, TFlashMode flashMode);
    /* Delegates */
    CSiaPlatformManager iSiaPlatformManager;
    CExtIspctlComponentManager iExtIspctlComponentManager;
    CExtIspctlCom iExtIspctlCom;
    CGrabControl iGrabControl;
    CGrab iGrab;
    CCapture_context iCapture_context;
    CDefferedEventMgr iDeferredEventMgr;
    COmxStateMgr iOmxStateMgr;
    CHostNmf_CB iCallback;
    CFlashDriver * iFlashDriver;
    /* Configuration at HSM construct level */
    /* Passed to the OpModeMgr instance */
    t_uint16 CapturePort;

    OMX_TICKS iPreviousTimeStamp;

    enumExtCameraSlot ExtCamSlot;

    /* Performances */
    void performances(OMX_BUFFERHEADERTYPE* pBuffer);
    unsigned long long mCurrFrameTime;
    unsigned long long mPrevFrameTime;
    unsigned long long mFrameInterval;
    unsigned long long mTotalFrameInterval;
    unsigned long long mFrameCount;
    float mInstantFps;
    float mMinFps;
    float mMaxFps;
    float mAvgFps;
    /* Statistics */
    int mStarvationCnt;
};

#endif /* _EXT_CAMERA_H_ */

