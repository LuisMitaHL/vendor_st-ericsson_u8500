/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define DBGT_DECLARE_AUTOVAR

#include "ext_camera.h"
#include "ext_omxcamera.h"

#include "OMX_IndexExt.h"

#ifdef ANDROID
#include "mmhwbuffer_ext.h"
#endif

#define FOCUS_STATUS_LOOP_MAX  50
#define FOCUS_STATUS_WAIT 50
#define CAMERA_FLASH_ON    15
#define CAMCORDER_FLASH_ON    (100+11)
#define FLASH_OFF    0

#undef DBGT_LAYER
#define DBGT_LAYER  0
#undef  DBGT_PREFIX
#define DBGT_PREFIX "CAM "
#include "debug_trace.h"

#ifndef MIN
    #define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
    #define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/* */
/* Camera::fillBufferDone : parent callback */
/* */
void Camera::fillBufferDone( OMX_BUFFERHEADERTYPE* pBuffer)
{
    /* Stop watchdog timer when frame is filled. */
    if(p_cam_sm->watchdogCounter == 0) {
        p_cam_sm->watchdogStop();
    }

    switch (pBuffer->nOutputPortIndex) {
    case (CAMERA_PORT_OUT0) :
        {
            // set the timestamp : use of the system time
            struct timespec t;
            t.tv_sec = t.tv_nsec = 0;
            clock_gettime(CLOCK_MONOTONIC, &t);
            pBuffer->nTimeStamp = (unsigned long long)(t.tv_sec)*1000000000LL + t.tv_nsec;

            performances(pBuffer);

            iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT0]--;
            fillBufferDoneVPB0(pBuffer);
            break;
        }
    case (CAMERA_PORT_OUT1) :
        {
            iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT1]--;
            fillBufferDoneVPB1(pBuffer);
            break;
        }
    default :
        DBC_ASSERT(0);
        break;
    }

    DBGT_PDEBUG("Remaining buffer at MPC side = %d\n", iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);

    /* Reload watchdog timer if some buffer are still at MPC side. */
    if((p_cam_sm->watchdogCounter == 0) && (iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex] != 0)) {
        p_cam_sm->watchdogStart();
    } else {
         DBGT_WARNING("MMDSP is in starvation. All buffer are at client side");
         mStarvationCnt++;
    }


    /* Reset the flushRequest boolean value */
    p_cam_sm->flushRequest = false;
}


/* */
/* Camera::fillBufferDoneVPB0 : Viewfinder call-back (StillPreview/VideoPreview) */
/* */
void Camera::fillBufferDoneVPB0(
        OMX_BUFFERHEADERTYPE* pOmxBufHdr)
{
    DBGT_PDEBUG("fillBufferDoneVPB0 >");

    if((pOmxBufHdr->nFlags!=OMX_BUFFERFLAG_EOS)
            &&(!iOpModeMgr.waitingEOS[CAMERA_PORT_OUT0])
            &&(iOpModeMgr.captureRequest[CAMERA_PORT_OUT1]==OMX_TRUE)
      )
    {
        //buffers are at components thus no EOS can be expected
        //flag it now as DSP wont do it
        DBGT_PTRACE("\t Autoflag EOS on VPB0 ");
        DBGT_PTRACE("iOpModeMgr.captureRequest[CAMERA_PORT_OUT1]=%d, iOpModeMgr.waitingEOS[CAMERA_PORT_OUT0]=%d",iOpModeMgr.captureRequest[CAMERA_PORT_OUT1],iOpModeMgr.waitingEOS[CAMERA_PORT_OUT0]);
        pOmxBufHdr->nFlags=OMX_BUFFERFLAG_EOS;
    }
    if ((pOmxBufHdr->nFlags!=OMX_BUFFERFLAG_EOS) && (iOpModeMgr.waitingEOS[CAMERA_PORT_OUT0] == OMX_TRUE)&&
            (iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT0]==0))
    {
        pOmxBufHdr->nFlags=OMX_BUFFERFLAG_EOS;
        iOpModeMgr.waitingEOS[CAMERA_PORT_OUT0]  = OMX_FALSE;
        /*inform grabctl also*/
        iGrabControl.setEndCaptureNow(CAMERA_PORT_OUT0);
    }

    OMX_STATETYPE currentState = OMX_StateInvalid;
    mENSComponent.GetState(&mENSComponent, &currentState);

    if((pOmxBufHdr->pAppPrivate != NULL)&&(currentState == OMX_StateExecuting)){
        struct timespec t1,t2;
        typedef struct deinterlace_info {
            void* src_hwmem_buf_name;
            void* dst_hwmem_buf_name;
            int width;
            int height;
        } deinterlace_info_t;

        deinterlace_info_t* info = (deinterlace_info_t*)pOmxBufHdr->pAppPrivate;

        t1.tv_sec = t1.tv_nsec = 0;
        t1.tv_sec = t2.tv_nsec = 0;
        clock_gettime(CLOCK_MONOTONIC, &t1);

        MMIO_Camera::deinterlaceFrame( (char*)pOmxBufHdr->pBuffer,
            info->src_hwmem_buf_name,
            info->dst_hwmem_buf_name,
            info->width,
            info->height );

        clock_gettime(CLOCK_MONOTONIC, &t2);
        DBGT_PDEBUG("deinterlace %dx%d time %lld ns", info->width, info->height,
            (unsigned long long)(t2.tv_sec-t1.tv_sec)*1000000000LL +  t2.tv_nsec - t1.tv_nsec);
    }

    /* __StillWOportdisa */
    if(pOmxBufHdr->nFlags==OMX_BUFFERFLAG_EOS)
    {
        DBGT_PTRACE("\t Self-triggered EOS on VPB0 ");

        iOpModeMgr.waitingEOS[CAMERA_PORT_OUT0]  = OMX_FALSE;

        /* VF port is enabled AND EOS for this port has come */
        /* reset EOS as we don't want it to be forwarded, it's not a real EOS at user point of view */
        pOmxBufHdr->nFlags = 0;

        /* dispatch the signal that will actually triggered the Capture now that EOS has been retrieved for the enabled port */
        DBGT_PTRACE("dispatch the signal");

        s_scf_event * event = iDeferredEventMgr.getEmptyEvent();
        event->sig = CAMERA_EOS_VPB02_SIG;
        iDeferredEventMgr.queuePriorEvent(event);

        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_cam_sm->ProcessEvent(&devent);

        mENSComponent.fillBufferDone(pOmxBufHdr);
    }
    else
    {
        mENSComponent.fillBufferDone(pOmxBufHdr);
    }

    DBGT_PDEBUG("fillBufferDoneVPB0 <");
}


/* */
/* Camera::fillBufferDoneVPB1 : Still/Burst Capture call-back */
/* */
void Camera::fillBufferDoneVPB1(
        OMX_BUFFERHEADERTYPE* pOmxBuf)
{
    DBGT_PDEBUG("fillBufferDoneVPB1 >");

    OMX_BOOL isOmxStateOk = OMX_BOOL((iOpModeMgr.CurrentOperatingMode != OpMode_Cam_StillPreview)&&(iOpModeMgr.CurrentOperatingMode !=OpMode_Cam_VideoPreview));
    if(isOmxStateOk) {

        OMX_U32 nBufferCountActual = mENSComponent.getPort(CAMERA_PORT_OUT1)->getBufferCountActual();
        if (iCapture_context.nfillBufferDoneCount < nBufferCountActual)
        {
            iCapture_context.ppOmxBufHdr[iCapture_context.nfillBufferDoneCount] = pOmxBuf;
        }
        iCapture_context.nfillBufferDoneCount++;

        s_scf_event * event = iDeferredEventMgr.getEmptyEvent();
        event->sig = CAMERA_FILLBUFFERDONE_SIG;
        iDeferredEventMgr.queuePriorEvent(event);

        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_cam_sm->ProcessEvent(&devent);
    } else
        mENSComponent.fillBufferDone(pOmxBuf);

    DBGT_PDEBUG("fillBufferDoneVPB1 <");
}


/* */
/* overwrites fillthisbuffer to count the number of buffers at MPC */
/* */
OMX_ERRORTYPE Camera::fillThisBuffer(
        OMX_BUFFERHEADERTYPE* pBuffer)
{
    DBGT_PDEBUG("fillThisBuffer >");

    if(iPanic==OMX_TRUE)
        return OMX_ErrorHardware;

    iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]++;

    mIfillThisBuffer[pBuffer->nOutputPortIndex].fillThisBuffer(pBuffer);

    /* Reload the watchdog timer if component in executing state. */
    OMX_STATETYPE currentState = OMX_StateInvalid;
    mENSComponent.GetState(&mENSComponent, &currentState);

    if(currentState == OMX_StateExecuting) {
        p_cam_sm->watchdogStart();
    }

    DBGT_PDEBUG("fillThisBuffer <");
    return OMX_ErrorNone;
}


/* */
/* Clock Callacks : obsolete */
/* */
void Camera::clockrunning(
        t_uint16 port_idx)
{
    DBGT_PROLOG("");
    DBGT_EPILOG("");
}

void Camera::clockwaitingforstart(
        t_uint16 port_idx)
{
    DBGT_PROLOG("");
    DBGT_EPILOG("");
}


void Camera::configured(
        t_uint16 port_idx)
{
    DBGT_PROLOG("");

    DBGT_PTRACE("Configured: port_idx = %d", port_idx);
    s_scf_event event;
    event.sig = CONFIGURE_SIG;
    event.type.other = port_idx;
    p_cam_sm->ProcessEvent(&event);

    DBGT_EPILOG("");
}


/* */
/* grab_api_alert callbacks */
/**/
void Camera::info(
        enum e_grabInfo info_id,
        t_uint16 buffer_id,
        t_uint32 timestamp)
{
    DBGT_PROLOG("");
    DBGT_PTRACE("grab_api_alert: info callback.");
    DBGT_EPILOG("");
}

void Camera::error(
        enum e_grabError error_id,
        t_uint16 data,
        t_uint16 buffer_id,
        t_uint32 timestamp)
{
    DBGT_PROLOG("");
    DBGT_PTRACE("grab_api_alert: error callback.");
    DBGT_EPILOG("");
}

void Camera::debug(
        enum e_grabDebug debug_id,
        t_uint16 buffer_id,
        t_uint16 data1,
        t_uint16 data2,
        t_uint32 timestamp)
{
    DBGT_PROLOG("");
    DBGT_PTRACE("grab_api_alert: debug callback.");
    DBGT_EPILOG("");
}


/* */
/* timer_api_alarm callbacks */
/* */
void Camera::signal(
        void)
{
    DBGT_PROLOG("");

    DBGT_WARNING("Camera::alarm => Camera does not respond for a while, watchdog of %dms trigged!!",p_cam_sm->watchdogIntervalMsec);

    /* Stop watchdog timer */
    p_cam_sm->watchdogStop();

    /* Increment the watchdog counter */
    p_cam_sm->watchdogCounter++;

    if (p_cam_sm->flushRequest == true) {
        DBGT_PTRACE("flush buffer requested then watchdog timeout occurs => empty the buffer located at MPC side.");
        p_cam_sm->pGrabControl->setForceDequeueBuffer(CAMERA_PORT_OUT0);
    }

    /* +ER372406 */
    /* Send error event to warn client that camera need to be restarted. */
    DBGT_WARNING("Camera streaming is not starting => client to restart camera.");
    mENSComponent.eventHandlerCB(OMX_EventError, (OMX_U32)OMX_ErrorTimeout, 0, 0);
    /* -ER372406 */

    DBGT_EPILOG("");
}


/* */
/* grabctl_api_alert callbacks */
/* */
void Camera::error(
        enum e_grabError error_id,
        t_uint16 data,
        enum e_grabPipeID pipe_id)
{
    DBGT_PROLOG("");

    switch(error_id){
    case IPP_CD_ERROR_CCP:
        DBGT_WARNING("grabctl error: CCP_ERROR on pipe_id=%d", pipe_id);
        break;
    case IPP_CD_ERROR_RAW_DATA_NOT_X8_BYTES:
        DBGT_WARNING("grabctl error: RAW_DATA_ERROR_NOT_X8_BYTES on pipe_id=%d", pipe_id);
        break;
    case IPP_CD_ERROR_RAW_DATA_OVERFLOW:
        DBGT_WARNING("grabctl error: RAW_DATA_ERROR_OVERFLOW on pipe_id=%d", pipe_id);
        break;
    case IPP_CD_ERROR_LUMA_NOT_X8_BYTES:
        DBGT_WARNING("grabctl error: LUMA_ERROR_NOT_X8_BYTES on pipe_id=%d", pipe_id);
        break;
    case IPP_CD_ERROR_LUMA_OVERFLOW:
        DBGT_WARNING("grabctl error: LUMA_ERROR_OVERFLOW on pipe_id=%d", pipe_id);
        break;
    case IPP_CD_ERROR_CHROMA_NOT_X16_BYTES_3BUF:
        DBGT_WARNING("grabctl error: CHROMA_ERROR_NOT_X16_BYTES_3BUF on pipe_id=%d", pipe_id);
        break;
    case IPP_CD_ERROR_CHROMA_NOT_X8_BYTES_2BUF:
        DBGT_WARNING("grabctl error: CHROMA_ERROR_NOT_X8_BYTES_2BUF on pipe_id=%d", pipe_id);
        break;
    case IPP_CD_ERROR_CHROMA_OVERFLOW:
        DBGT_WARNING("grabctl error: CHROMA_ERROR_OVERFLOW on pipe_id=%d ", pipe_id);
        break;
    case IPP_CD_CSI2_DPHY_ERROR:
        DBGT_WARNING("grabctl error: CSI2_DPHY_ERROR on pipe_id=%d", pipe_id);
        if((data & 0x0001) == 0x0001)
            DBGT_WARNING("grabctl error: CSI0_ERR_EOT_SYNC_HS_DL1 (data=0x%X)", data);
        if((data & 0x0002) == 0x0002)
            DBGT_WARNING("grabctl error: CSI0_ERR_EOT_SYNC_HS_DL2 (data=0x%X)", data);
        if((data & 0x0004) == 0x0004)
            DBGT_WARNING("grabctl error: CSI0_ERR_EOT_SYNC_HS_DL3 (data=0x%X)", data);
        if((data & 0x0008) == 0x0008)
            DBGT_WARNING("grabctl error: CSI0_ERR_SOT_HS_DL1 (data=0x%X)", data);
        if((data & 0x0010) == 0x0010)
            DBGT_WARNING("grabctl error: CSI0_ERR_SOT_HS_DL2 (data=0x%X)", data);
        if((data & 0x0020) == 0x0020)
            DBGT_WARNING("grabctl error: CSI0_ERR_SOT_HS_DL3 (data=0x%X)", data);
        if((data & 0x0040) == 0x0040)
            DBGT_WARNING("grabctl error: CSI0_ERR_SOT_SYNC_HS_DL1 (data=0x%X)", data);
        if((data & 0x0080) == 0x0080)
            DBGT_WARNING("grabctl error: CSI0_ERR_SOT_SYNC_HS_DL2 (data=0x%X)", data);
        if((data & 0x0100) == 0x0100)
            DBGT_WARNING("grabctl error: CSI0_ERR_SOT_SYNC_HS_DL3 (data=0x%X)", data);
        if((data & 0x0200) == 0x0200)
            DBGT_WARNING("grabctl error: CSI0_ERR_CONTROL_DL1 (data=0x%X)", data);
        if((data & 0x0400) == 0x0400)
            DBGT_WARNING("grabctl error: CSI0_ERR_CONTROL_DL2 (data=0x%X)", data);
        if((data & 0x0800) == 0x0800)
            DBGT_WARNING("grabctl error: CSI0_ERR_CONTROL_DL3 (data=0x%X)", data);
        if((data & 0x1000) == 0x1000)
            DBGT_WARNING("grabctl error: CSI1_ERR_EOT_SYNC_HS_DL1 (data=0x%X)", data);
        if((data & 0x2000) == 0x2000)
            DBGT_WARNING("grabctl error: CSI1_ERR_SOT_HS_DL1 (data=0x%X)", data);
        if((data & 0x4000) == 0x4000)
            DBGT_WARNING("grabctl error: CSI1_ERR_SOT_SYNC_HS_DL1 (data=0x%X)", data);
        if((data & 0x8000) == 0x8000)
            DBGT_WARNING("grabctl error: CSI1_ERR_CONTROL_DL1 (data=0x%X)", data);
        break;
    case IPP_CD_CSI2_PACKET_ERROR:
        DBGT_WARNING("grabctl error: CSI2_PACKET_ERROR on pipe_id=%d", pipe_id);
        if((data & 0x0001) == 0x0001)
            DBGT_WARNING("grabctl error: CSI2_PACKET_PAYLOAD_CRC_ERROR (data=0x%X)", data);
        if((data & 0x0002) == 0x0002)
            DBGT_WARNING("grabctl error: CSI2_PACKET_HEADER_ECC_ERROR (data=0x%X)", data);
        if((data & 0x0004) == 0x0004)
            DBGT_WARNING("grabctl error: CSI2_EOF (data=0x%X) (This is not an error, it just means that EOF has been received)", data);
        break;
    case GRBERR_GRB_IN_PROGRESS:
        DBGT_CRITICAL("grabctl error: GRBERR_GRB_IN_PROGRESS on pipe_id=%d", pipe_id);
        break;
    case GRBERR_WATCHDOG_RELOAD:
        DBGT_PDEBUG("Watchdog to reload due to frame skip");
        p_cam_sm->watchdogStop();
        p_cam_sm->watchdogStart();
        break;
    default:
        break;
    }

    DBGT_EPILOG("");
}


/* */
/* doSpecificEventHandler_cb is used in case where the Camera has to dispatch/delay the callback handler */
/* */
OMX_ERRORTYPE Camera::doSpecificEventHandler_cb(
        OMX_EVENTTYPE ev,
        OMX_U32 data1,
        OMX_U32 data2,
        OMX_BOOL & bDeferredEventHandler)
{
    DBGT_PROLOG("");

    OMX_STATETYPE exitedState = OMX_StateInvalid;
    mENSComponent.GetState(&mENSComponent, &exitedState);


    bDeferredEventHandler = OMX_FALSE;
    switch(ev){
    case OMX_EventCmdComplete:
        {
            if(data1 == OMX_CommandStateSet)
            {
                if (data2 == OMX_StateIdle && exitedState == OMX_StateExecuting)
                {
                    DBGT_PTRACE("OMX_CommandStateSet : exitedState=%d, currentState=%d", (int)exitedState,(int)data2);
                    s_scf_event * event = iDeferredEventMgr.getEmptyEvent();
                    event->sig = CAMERA_EXECUTING_TO_IDLE_END_SIG;
                    iDeferredEventMgr.queueNewEvent(event);
                    s_scf_event devent;
                    devent.sig = Q_DEFERRED_EVENT_SIG;
                    p_cam_sm->ProcessEvent(&devent);
                    bDeferredEventHandler = OMX_TRUE;
                }
                else if (data2 == OMX_StateIdle && exitedState == OMX_StateLoaded)
                {
                    DBGT_PTRACE("OMX_CommandStateSet : exitedState=%d, currentState=%d", (int)exitedState,(int)data2);
                    /* Construct state machine */
                    constructSM(mENSComponent);
                    /* Initialize the camera state machine. */
                    SCF_START_SM(p_cam_sm);
                    bDeferredEventHandler = OMX_TRUE;
                }
                else if (data2 == OMX_StateExecuting && exitedState == OMX_StateIdle) {
                    DBGT_PTRACE("OMX_CommandStateSet : exitedState=%d, currentState=%d", (int)exitedState,(int)data2);
                    s_scf_event * event = iDeferredEventMgr.getEmptyEvent();
                    event->sig = CAMERA_IDLE_TO_EXECUTING_SIG;
                    iDeferredEventMgr.queueNewEvent(event);
                    s_scf_event devent;
                    devent.sig = Q_DEFERRED_EVENT_SIG;
                    p_cam_sm->ProcessEvent(&devent);
                    bDeferredEventHandler = OMX_TRUE;

                    /* Recover the watchdog timeout if setted in the driver file */
                    p_cam_sm->watchdogIntervalMsec = cam_Ctrllib->Camera_CtrlLib_GetWatchdogTimeout(cam_h);
                }
            }
            else if (data1 == OMX_CommandPortEnable)
            {
                DBGT_PTRACE("OMX_CommandPortEnable : exitedState=%d", exitedState);
                if((exitedState == OMX_StateIdle)||(exitedState == OMX_StateExecuting))
                {
                    bDeferredEventHandler = OMX_FALSE;

                    if(cam_Ctrllib->Camera_CtrlLib_GetPipeNumber(cam_h) > 1)
                    {
                        if( data2 == 1){  //still port
                            int DelayTime = 0;
                            int bSuccess = false;
                            //Configure for right size
                            camport * port = (camport *)  mENSComponent.getPort(CapturePort);

                            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
                            mParamPortDefinition.nSize               = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
                            mENSComponent.getPort(port->getPortIndex())->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

                            HAL_Gr_Camera_SizeConfig_t* SizeConfig_p = new HAL_Gr_Camera_SizeConfig_t;
                            t_uint8 Id = 0;
                            bool found = 0;

                            while(!found)
                            {
                                bSuccess =  cam_Ctrllib->Camera_CtrlLib_GetSizeConfig( cam_h, Id, SizeConfig_p);
                                if(!bSuccess){
                                    DBGT_CRITICAL("Error from Camera_CtrlLib_GetSizeConfig: Id not found");
                                    break;
                                }
                                if((SizeConfig_p->Height == (int)mParamPortDefinition.format.image.nFrameHeight) &&
                                        (SizeConfig_p->Width == (int)mParamPortDefinition.format.image.nFrameWidth) &&
                                        (SizeConfig_p->Type== 0)) /*For raw capture table contains 0*/
                                    found = 1;
                                else
                                    Id++;
                            }

                            bSuccess =  cam_Ctrllib->Camera_CtrlLib_SetSnapshotSize( cam_h, Id, &DelayTime);
                            if(!bSuccess)
                            {
                                DBGT_CRITICAL("Error from Camera_CtrlLib_SetSnapshotSize");
                            }
                        }
                    }

                }
            }
        }
        break;
    case OMX_EventError:
        {
            DBGT_PTRACE("OMX_EventError: data1=%lu data2=%lu", data1, data2);
            break;
        }
    default:
        break;
    }

    DBGT_EPILOG("");

    return OMX_ErrorNone;
}



/* */
/* Construct HSMCAM */
/* */
void Camera::constructSM(
        ENS_Component &enscomp)
{
    DBGT_PROLOG("");

    DBGT_PTRACE("SM from Camera is initialized and started");
    p_cam_sm = new CAM_SM(enscomp);

    p_cam_sm->CapturePort                = CapturePort;
    p_cam_sm->pExtIspctlCom              = &iExtIspctlCom;
    p_cam_sm->pDeferredEventMgr          = &iDeferredEventMgr;
    p_cam_sm->pOmxStateMgr               = &iOmxStateMgr;
    p_cam_sm->pOpModeMgr                 = &iOpModeMgr;
    p_cam_sm->pExtIspctlComponentManager = &iExtIspctlComponentManager;
    p_cam_sm->pGrabControl               = &iGrabControl;
    p_cam_sm->pGrab                      = &iGrab;
    p_cam_sm->pCapture_context           = &iCapture_context;
    p_cam_sm->HSMExtCamSlot              = ExtCamSlot;
    p_cam_sm->watchdogIntervalMsec       = 0;    // Watchdog deactivated by default
    p_cam_sm->isWatchdogTimerRunning     = false;
    p_cam_sm->mTimer                     = mTimer;
    p_cam_sm->watchdogCounter            = 0;
    p_cam_sm->flushRequest               = false;

    iExtIspctlComponentManager.setSM((CScfStateMachine*)p_cam_sm);

    DBGT_EPILOG("");
}

/* */
/* Destroy CAM_SM */
/* */
void Camera::destroySM(
        void)
{
    DBGT_PROLOG("");

    delete p_cam_sm;

    DBGT_EPILOG("");
}

int Camera::configureIPP(
        int enable)
{
    t_uint8 selCam      = CAMCTRL_CAM_SEL_SECONDARY;
    t_uint8 selDataIntf = CAMCTRL_CAM_DATA_INTF_CSI2;
    t_uint8 numOfLanes  = 0;

    /*Set the selected camera*/
    if(eSecondaryExtCamera == ExtCamSlot) {
        //Secondry camera is selected
        selCam = CAMCTRL_CAM_SEL_SECONDARY;
    } else {
        //Primary camera is selected
        selCam = CAMCTRL_CAM_SEL_PRIMARY;
    }

    if(HW_DPHY_ENABLE == enable)
    {
        DBGT_PTRACE("Enable the DPHY");
        CamCtrl_CsiIntf_Config_t csiConfig;

        //Get the data interface detail
        if(TRUE != cam_Ctrllib->Camera_CtrlLib_Get_cam_interface_config(cam_Ctrllib->Camera_CtrlLib_GetDriverHandle(), &selCam, &selDataIntf, &numOfLanes, &csiConfig)) {
            DBGT_PTRACE("Error: Failed to get cam interface configuration");
        }

        if(ExtCamSlot == eSecondaryExtCamera) {
            struct s_CcpConfig Secondary_interface_ccpConfig;
            struct s_CsiConfig Secondary_interface_csiConfig;

            memset((char*)&Secondary_interface_ccpConfig,0,sizeof(struct s_CcpConfig));
            memset((char*)&Secondary_interface_csiConfig,0,sizeof(struct s_CsiConfig));

            if(CAMCTRL_CAM_DATA_INTF_CCP2 == selDataIntf) {
                int success = cam_Ctrllib->Camera_CtrlLib_Get_ccp2_config(
                    cam_Ctrllib->Camera_CtrlLib_GetDriverHandle(),
                    &(Secondary_interface_ccpConfig.Ccp_Static_BPP),
                    &(Secondary_interface_ccpConfig.Ccp_Static_Data_Strobe) );
                if (success) {
                    Secondary_interface_csiConfig.Dphy_interface_enable = 1;
                    ((&iExtIspctlComponentManager)->getExtIspctlCfg()).Secondary_Interface_Configuration(
                        SensorType_CCP,
                        Secondary_interface_csiConfig,
                        Secondary_interface_ccpConfig );
                } else {
                    DBGT_PTRACE("Error: CCP config info cannot be retrieved, %d", success);
                }
            } else {
                //set to shift down the value of HS RX termination
                Secondary_interface_csiConfig.Dphy_clock_Lane_Control_HSRX_Term_Shift_Down_Cl = 0x0;
                //set to shift up the value of HS RX termination
                Secondary_interface_csiConfig.Dphy_clock_Lane_Control_HSRX_Term_Shift_Up_Cl = 0x0;
                //set to invert only P and N pins for high speed
                Secondary_interface_csiConfig.Dphy_clock_Lane_Control_Hs_Invert_Cl = 0x0;
                //set to invert P and N pins both for high-speed and low power CSI2 modes.
                //In CCP2 SMIA mode, it is set to invert clk/strobe edge fro data sampling.
                Secondary_interface_csiConfig.Dphy_clock_Lane_Control_Swap_Pin_Cl = 0x0;
                //set the IP mode wrt the MIPI DPHY specification (1: v0.90, 0: v0.81)
                if(csiConfig.Mipi_Specs_90_81b == 0x1)
                {
                    Secondary_interface_csiConfig.Dphy_clock_Lane_Control_Mipi_Specs_90_81b = 0x1;
                }
                else
                {
                    /* Set v.81b DPHY spec even if older DPHY spec is used. */
                    Secondary_interface_csiConfig.Dphy_clock_Lane_Control_Mipi_Specs_90_81b = 0x0;
                }
                //set according to the sensor data bitrate, in Mbps. Given N is the Mbps, formula is csi0_ui_x4= round_down(1000/N *4)
                //e.g a N=600 Mbps is given by a csi0_ui_x4=round_down(1000/600*4)=round_down(6.67)=6.
                Secondary_interface_csiConfig.Dphy_clock_Lane_Control_Ui_X4 = (t_uint16) ((1000 * 4) / csiConfig.CSI_bitrate_mbps);

                if (csiConfig.Eot_Bypass == 0x1) {
                    /* Enable EOT_bypass */
                    //drives signals to be connected to CSI2 DPHY1 data lane 1
                    Secondary_interface_csiConfig.Dphy_data_Lane1_Control = 0x18;
                } else {
                    //drives signals to be connected to CSI2 DPHY1 data lane 1
                    Secondary_interface_csiConfig.Dphy_data_Lane1_Control = 0x8;
                }

                //Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module
                Secondary_interface_csiConfig.Dphy_Static_CSI2RX_DataType0 = csiConfig.CSI_DataType0;
                Secondary_interface_csiConfig.Dphy_Static_CSI2RX_DataType1 = csiConfig.CSI_DataType1;
                Secondary_interface_csiConfig.Dphy_Static_CSI2RX_DataType2 = csiConfig.CSI_DataType2;
                DBGT_PTRACE("Secondary datatype supported => DataType0 =0x%X, DataType1 =0x%X, DataType2 =0x%X",
                    csiConfig.CSI_DataType0, csiConfig.CSI_DataType1, csiConfig.CSI_DataType2);

                //Static register used to set the configuration of the CSI2RX byte_to_pixel
                //HW sub-module with the values of the embedded data and active data pixel widths
                Secondary_interface_csiConfig.Dphy_Static_CSI2RX_DataType0_Pixel_Width = csiConfig.CSI_DataType0_PixWidth;
                Secondary_interface_csiConfig.Dphy_Static_CSI2RX_DataType1_Pixel_Width = csiConfig.CSI_DataType1_PixWidth;
                Secondary_interface_csiConfig.Dphy_Static_CSI2RX_DataType2_Pixel_Width = csiConfig.CSI_DataType2_PixWidth;

                Secondary_interface_csiConfig.Dphy_interface_enable = 1;
                ((&iExtIspctlComponentManager)->getExtIspctlCfg()).Secondary_Interface_Configuration(
                    SensorType_CSI,
                    Secondary_interface_csiConfig,
                    Secondary_interface_ccpConfig );
            }
        }
        else //Primary camera is selected
        {
            struct s_CsiConfig Primary_interface_csiConfig;

            memset((char*)&Primary_interface_csiConfig,0,sizeof(struct s_CsiConfig));

            if (numOfLanes == 4) {
                //select and map the 4 output physical DPHY data lanes onto the 4 input logical CSI2RX0 data lanes
                Primary_interface_csiConfig.Dphy_Data_Lanes_Map = 0x8D1;
            } else {
                //select and map the 3 output physical DPHY data lanes onto the 3 input logical CSI2RX0 data lanes
                Primary_interface_csiConfig.Dphy_Data_Lanes_Map = 0xD1;
            }
            //set the number of data lanes one, two, or three
            Primary_interface_csiConfig.Dphy_Data_Lanes_Number = numOfLanes;
            //set to shift down the value of HS RX termination
            Primary_interface_csiConfig.Dphy_clock_Lane_Control_HSRX_Term_Shift_Down_Cl = 0x0;
            //set to shift up the value of HS RX termination
            Primary_interface_csiConfig.Dphy_clock_Lane_Control_HSRX_Term_Shift_Up_Cl = 0x0;
            //set to invert only P and N pins for high speed
            Primary_interface_csiConfig.Dphy_clock_Lane_Control_Hs_Invert_Cl = 0x0;
            //set to invert P and N pins both for high-speed and low power CSI2 modes.
            //In CCP2 SMIA mode, it is set to invert clk/strobe edge fro data sampling.
            Primary_interface_csiConfig.Dphy_clock_Lane_Control_Swap_Pin_Cl = 0x0;
            //set the IP mode wrt the MIPI DPHY specification (1: v0.90, 0: v0.81)
            if(csiConfig.Mipi_Specs_90_81b == 0x1)
            {
                Primary_interface_csiConfig.Dphy_clock_Lane_Control_Mipi_Specs_90_81b = 0x1;
            }
            else
            {
                /* Set v.81b DPHY spec even if older DPHY spec is used. */
                Primary_interface_csiConfig.Dphy_clock_Lane_Control_Mipi_Specs_90_81b = 0x0;
            }
            //set according to the sensor data bitrate, in Mbps. Given N is the Mbps, formula is csi0_ui_x4= round_down(1000/N *4)
            //e.g a N=600 Mbps is given by a csi0_ui_x4=round_down(1000/600*4)=round_down(6.67)=6.
            Primary_interface_csiConfig.Dphy_clock_Lane_Control_Ui_X4 = (t_uint16) ((1000 * 4) / csiConfig.CSI_bitrate_mbps);

            if (csiConfig.Eot_Bypass == 0x1) {
                /* Enable EOT_bypass */
                //drives signals to be connected to CSI2 DPHY0 data lane 1
                Primary_interface_csiConfig.Dphy_data_Lane1_Control = 0x18;
                //drives signals to be connected to CSI2 DPHY0 data lane 2
                Primary_interface_csiConfig.Dphy_data_Lane2_Control = 0x18;
                //drives signals to be connected to CSI2 DPHY0 data lane 3
                Primary_interface_csiConfig.Dphy_data_Lane3_Control = 0x18;
                //drives signals to be connected to CSI2 DPHY0 data lane 4
                if (numOfLanes == 4) Primary_interface_csiConfig.Dphy_data_Lane4_Control = 0x18;
            } else {
                //drives signals to be connected to CSI2 DPHY0 data lane 1
                Primary_interface_csiConfig.Dphy_data_Lane1_Control = 0x8;
                //drives signals to be connected to CSI2 DPHY0 data lane 2
                Primary_interface_csiConfig.Dphy_data_Lane2_Control = 0x8;
                //drives signals to be connected to CSI2 DPHY0 data lane 3
                Primary_interface_csiConfig.Dphy_data_Lane3_Control = 0x8;
                //drives signals to be connected to CSI2 DPHY0 data lane 4
                if (numOfLanes == 4) Primary_interface_csiConfig.Dphy_data_Lane4_Control = 0x8;
            }

            //Static register used to set the configuration of the CSI2RX byte_to_pixel HW sub-module
            Primary_interface_csiConfig.Dphy_Static_CSI2RX_DataType0 = csiConfig.CSI_DataType0;
            Primary_interface_csiConfig.Dphy_Static_CSI2RX_DataType1 = csiConfig.CSI_DataType1;
            Primary_interface_csiConfig.Dphy_Static_CSI2RX_DataType2 = csiConfig.CSI_DataType2;
            DBGT_PTRACE("Primary datatype supported => DataType0 =0x%X, DataType1 =0x%X, DataType2 =0x%X", csiConfig.CSI_DataType0, csiConfig.CSI_DataType1, csiConfig.CSI_DataType2);

            //Static register used to set the configuration of the CSI2RX byte_to_pixel
            //HW sub-module with the values of the embedded data and active data pixel widths
            Primary_interface_csiConfig.Dphy_Static_CSI2RX_DataType0_Pixel_Width = csiConfig.CSI_DataType0_PixWidth;
            Primary_interface_csiConfig.Dphy_Static_CSI2RX_DataType1_Pixel_Width = csiConfig.CSI_DataType1_PixWidth;
            Primary_interface_csiConfig.Dphy_Static_CSI2RX_DataType2_Pixel_Width = csiConfig.CSI_DataType2_PixWidth;

            Primary_interface_csiConfig.Dphy_interface_enable = 1;
            ((&iExtIspctlComponentManager)->getExtIspctlCfg()).Primary_Interface_Configuration(Primary_interface_csiConfig);
        }
    }
    else if(HW_DPHY_DISABLE == enable)
    {
        DBGT_PTRACE("Disable the DPHY");

        //Need to disable IPP
        if(ExtCamSlot == eSecondaryExtCamera) {
            struct s_CcpConfig Secondary_interface_ccpConfig;
            struct s_CsiConfig Secondary_interface_csiConfig;

            memset((char*)&Secondary_interface_ccpConfig,0,sizeof(struct s_CcpConfig));
            memset((char*)&Secondary_interface_csiConfig,0,sizeof(struct s_CsiConfig));

            Secondary_interface_csiConfig.Dphy_interface_enable = 0;
            ((&iExtIspctlComponentManager)->getExtIspctlCfg()).Secondary_Interface_Configuration(
                    SensorType_CSI, Secondary_interface_csiConfig,
                    Secondary_interface_ccpConfig );
        } else {
            struct s_CsiConfig Primary_interface_csiConfig;

            memset((char*)&Primary_interface_csiConfig,0,sizeof(struct s_CsiConfig));

            Primary_interface_csiConfig.Dphy_interface_enable = 0;
            ((&iExtIspctlComponentManager)->getExtIspctlCfg()).Primary_Interface_Configuration(Primary_interface_csiConfig);
        }
    }

    return 1;
}

/* */
/* Processing Component constructor */
/* */
Camera::Camera(ENS_Component &enscomp, enumExtCameraSlot cam) :
    IFM_HostNmfProcessingComp(enscomp)
    ,iOpModeMgr()
    ,iPanic(OMX_FALSE)
    ,instantiateDone(OMX_FALSE)
    ,startDone(OMX_FALSE)
    ,cam_Ctrllib(NULL)
    ,cam_h(0)
    ,iSiaPlatformManager()
    ,iExtIspctlComponentManager(SIA_CLIENT_CAMERA, enscomp)
    ,iExtIspctlCom(SIA_CLIENT_CAMERA)
    ,iGrabControl(enscomp)
    ,iGrab()
    ,iCapture_context()
    ,iDeferredEventMgr()
    ,iOmxStateMgr()
    ,iCallback(this)
    ,CapturePort(CAMERA_PORT_OUT1)
    ,mCurrFrameTime(0)
    ,mPrevFrameTime(0)
    ,mFrameInterval(0)
    ,mTotalFrameInterval(0)
    ,mFrameCount(0)
    ,mInstantFps(0.0)
    ,mMinFps(0.0)
    ,mMaxFps(0.0)
    ,mAvgFps(0.0)
    ,mStarvationCnt(0)
{
    DBGT_TRACE_INIT(extcamera);

    DBGT_PROLOG("");
    af_thread_t = 0;
    pthread_mutex_init( &mutex, NULL);
    m_auto_flash_on = 0;
    m_is_need_flash_on = 0;
    m_flash_control = OMX_IMAGE_FlashControlAuto;

    if((cam == ePrimaryExtCamera) || (cam == eSecondaryExtCamera)) {
        ExtCamSlot = cam;
    } else {
        // A target device must have been set in CameraFactoryMethod().
        DBC_ASSERT(0);
    }

    iOpModeMgr.Init(CapturePort);

    /* default configuration */
    OMX_PARAM_PORTDEFINITIONTYPE defaultParam;

    defaultParam.nPortIndex                = CAMERA_PORT_OUT0;
    defaultParam.format.video.nFrameWidth  = camport::DEFAULT_WIDTH;
    defaultParam.format.video.nFrameHeight = camport::DEFAULT_HEIGHT;
    defaultParam.format.video.nSliceHeight = camport::DEFAULT_HEIGHT;
    defaultParam.format.video.eColorFormat = camport::DEFAULT_COLOR_FORMAT;
    defaultParam.format.video.xFramerate   = camport::DEFAULT_FRAMERATE;
    camport* port = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT0);
    port->setDefaultFormatInPortDefinition(&defaultParam);

    defaultParam.nPortIndex                = CAMERA_PORT_OUT1;
    defaultParam.format.image.nFrameWidth  = camport::DEFAULT_WIDTH;
    defaultParam.format.image.nFrameHeight = camport::DEFAULT_HEIGHT;
    defaultParam.format.image.nSliceHeight = camport::DEFAULT_HEIGHT;
    defaultParam.format.image.eColorFormat = camport::DEFAULT_COLOR_FORMAT;
    port = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT1);
    port->setDefaultFormatInPortDefinition(&defaultParam);

    iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT0]=0;
    iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT1]=0;

    // board & sensor init
    if (iSiaPlatformManager.init(ExtCamSlot) != OMX_ErrorNone){
        DBC_ASSERT(0);
    }

    if (ExtCamSlot == ePrimaryExtCamera)
        cam_Ctrllib = new Camera_Ctrllib(TRUE);
    else
        cam_Ctrllib = new Camera_Ctrllib(FALSE);

    int err = cam_Ctrllib->Camera_CtrlLib_LoadDriver(mENSComponent.getOMXHandle());
    if(err)
    {
        DBGT_PTRACE("Error: Camera_CtrlLib_LoadDriver failed");
    }

    COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
    cam_h = cam_Ctrllib->Camera_CtrlLib_GetDriverHandle();

    #define OMX_VERSION_1_1_2 0x00020101
    OMXCam->mLensParamType.nSize = sizeof(OMX_STE_CONFIG_LENSPARAMETERTYPE);
    OMXCam->mLensParamType.nVersion.nVersion = OMX_VERSION_1_1_2; // OMX IL 1.1.2

    int bSuccess = cam_Ctrllib->Camera_CtrlLib_Get_LensParameters(
        cam_h,
        (int*)&OMXCam->mLensParamType.nHorizontolViewAngle,
        (int*)&OMXCam->mLensParamType.nVerticalViewAngle,
        (int*)&OMXCam->mLensParamType.nFocalLength );

    if(!bSuccess) {
        DBGT_CRITICAL("Error in getting LensParam");
    }

    /* Flash driver init */
    iFlashDriver = CFlashDriver::Open();
    if (NULL == iFlashDriver){
        DBGT_CRITICAL("Error: could not open flash driver.\n");
    }

    bSuccess = cam_Ctrllib->Camera_CtrlLib_Get_Supported_Zoom_Resolutions(
           cam_h,
           (int*)&OMXCam->mZoomSupportedRes.nPreviewWidth,
           (int*)&OMXCam->mZoomSupportedRes.nPreviewHeight,
           (int*)&OMXCam->mZoomSupportedRes.nPictureWidth,
           (int*)&OMXCam->mZoomSupportedRes.nPictureHeight);

    if(!bSuccess) {
        DBGT_CRITICAL("Error in getting Get_Supported_Zoom_Resolutions");
    }
    DBGT_EPILOG("");
}


/* */
/* setOperatingMode
*/
OMX_ERRORTYPE Camera::setOperatingMode(
        t_operating_mode_camera aOpMode)
{
    iOpModeMgr.CurrentOperatingMode = aOpMode;

    return OMX_ErrorNone;
}


/* */
/* Processing Component destructor
*/
Camera::~Camera()
{
    DBGT_PROLOG("");

    /* +ER363747 */
    // check if the stop must be done
    if( startDone == OMX_TRUE ){
        stop();
    }

    // check if the deInstantiate must be done
    if( instantiateDone == OMX_TRUE ){
        deInstantiate();
    }
    /* -ER363747 */

    if(iFlashDriver){
      iFlashDriver->Close();
      iFlashDriver = NULL;
    }

    // board & sensor deinit
    if (iSiaPlatformManager.deinit(ExtCamSlot) != OMX_ErrorNone)
        DBGT_PTRACE("board & sensor deinit failed");

    cam_Ctrllib->Camera_CtrlLib_UnloadDriver(cam_h, mENSComponent.getOMXHandle());
    delete cam_Ctrllib;
    cam_Ctrllib = NULL;

    pthread_mutex_destroy(&mutex);

    DBGT_PTRACE("Camera deleted");

    DBGT_EPILOG("");
}


/* */
/* Not used */
/* */
OMX_ERRORTYPE Camera::emptyThisBuffer(
        OMX_BUFFERHEADERTYPE* pBuffer)
{
    DBGT_PTRACE("Camera has no input port");
    return OMX_ErrorNone;
}


/* */
/* Camera::getConfig */
/* */
OMX_ERRORTYPE Camera::retrieveConfig(
        OMX_INDEXTYPE aIdx,
        OMX_PTR p)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err=OMX_ErrorNone;
    DBGT_PTRACE("Camera::retrieveConfig: param aIdx is 0x%x",aIdx);

    switch ((OMX_U32)aIdx) {
    case OMX_IndexConfigCommonFocusStatus:
        {
            OMX_PARAM_FOCUSSTATUSTYPE *pFocusStatusType = (OMX_PARAM_FOCUSSTATUSTYPE *) p;
            int nFocusStatus=0;
            cam_Ctrllib->Camera_CtrlLib_GetFocusStatus(cam_h, &nFocusStatus);

            DBGT_PTRACE("retrieved configuration: %d", nFocusStatus);
            pFocusStatusType->eFocusStatus = (OMX_FOCUSSTATUSTYPE)nFocusStatus;
            break;
        }
    case OMX_IndexConfigFocusControl:
        {
            OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE *pFocusControlType = (OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE *) p;
            int nFocusControl=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(
                    cam_h, CAMCTRL_FOCUS_MODES, &nFocusControl);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting focus control type");
            }
            DBGT_PTRACE("retrieved configuration: %d", nFocusControl);
            pFocusControlType->eFocusControl = (OMX_IMAGE_FOCUSCONTROLTYPE) nFocusControl;
            break;
        }
    case OMX_Symbian_IndexConfigFocusRange:
        {
            OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE *pFocusRangeType = (OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE *) p;
            int nFocusRange=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(
                    cam_h, CAMCTRL_FOCUS_RANGE, &nFocusRange);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting focus range type");
            }
            DBGT_PTRACE("retrieved configuration: %d", nFocusRange);
            pFocusRangeType->eFocusRange = (OMX_SYMBIAN_FOCUSRANGETYPE) nFocusRange;
            break;
        }
    case OMX_Symbian_IndexConfigCommonExtCapturing:
        {
            OMX_SYMBIAN_CONFIG_BOOLEANTYPE *pCaptureConfig = (OMX_SYMBIAN_CONFIG_BOOLEANTYPE *)p;
            pCaptureConfig->bEnabled = iOpModeMgr.ConfigCapturing[pCaptureConfig->nPortIndex];
            break;
        }
    case OMX_IndexAutoPauseAfterCapture:
        {
            OMX_CONFIG_BOOLEANTYPE *pAutopauseConfig = (OMX_CONFIG_BOOLEANTYPE *)p;
            pAutopauseConfig->bEnabled = iOpModeMgr.AutoPauseAfterCapture;
            break;
        }
    case OMX_IndexConfigCaptureMode:
        {
            OMX_CONFIG_CAPTUREMODETYPE *pCaptureModeConfig = (OMX_CONFIG_CAPTUREMODETYPE *)p;
            pCaptureModeConfig->bFrameLimited = iOpModeMgr.IsBurstLimited() ? OMX_TRUE : OMX_FALSE;
            pCaptureModeConfig->nFrameLimit   = iOpModeMgr.GetBurstFrameLimit();
            break;
        }
    case OMX_IndexConfigCommonDigitalZoom:
        {
            OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE pZoomConfig = *(OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)p;
            break;
        }
    case OMX_IndexConfigCommonWhiteBalance:
        {
            OMX_CONFIG_WHITEBALCONTROLTYPE *pWhiteBalConfig = (OMX_CONFIG_WHITEBALCONTROLTYPE *)p;
            int whitebalance = 0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_WHITE_BALANCE, &whitebalance);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting whitebalance");
            }
            DBGT_PTRACE("retrieved whitebalance configuration: %d", whitebalance);
            pWhiteBalConfig->eWhiteBalControl = (OMX_WHITEBALCONTROLTYPE)whitebalance;
            break;
        }
    case OMX_IndexConfigCommonBrightness:
        {
            OMX_CONFIG_BRIGHTNESSTYPE *pBrightConfig = (OMX_CONFIG_BRIGHTNESSTYPE *) p;
            int brightness=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(
                    cam_h, CAMCTRL_BRIGHTNESS, &brightness);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting brightness");
            }
            DBGT_PTRACE("retrieved brightness configuration: %d", brightness);
            pBrightConfig->nBrightness = brightness;
            break;
        }
    case OMX_IndexConfigCommonExposureValue:
        {
            OMX_CONFIG_EXPOSUREVALUETYPE *pExposureValue = (OMX_CONFIG_EXPOSUREVALUETYPE *) p;
            int exposurecompensation = 0;
            int metering_mode = 0;
            int iso = 0;
            int bSuccess =
                cam_Ctrllib->Camera_CtrlLib_GetProperty(
                        cam_h, CAMCTRL_EXPOSURE_COMPENSATION, &exposurecompensation);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting exposure compensation");
            }
            DBGT_PTRACE("retrieved exposure configuration: %d", exposurecompensation);
            pExposureValue->xEVCompensation = exposurecompensation;

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_GetProperty(
                        cam_h, CAMCTRL_ISO, &iso);
            if(!bSuccess){
                DBGT_WARNING("Error in getting iso sensitivity");
            }

            if (iso == CAMCTRL_ISO_AUTO){
                pExposureValue->bAutoSensitivity = (OMX_BOOL)true;
            } else {
                pExposureValue->bAutoSensitivity = (OMX_BOOL)false;
                switch(iso){
                case CAMCTRL_ISO_100:
                    pExposureValue->nSensitivity = 100;
                    break;
                case CAMCTRL_ISO_200:
                    pExposureValue->nSensitivity = 200;
                    break;
                case CAMCTRL_ISO_400:
                    pExposureValue->nSensitivity = 400;
                    break;
                case CAMCTRL_ISO_800:
                    pExposureValue->nSensitivity = 800;
                    break;
                case CAMCTRL_ISO_1600:
                    pExposureValue->nSensitivity = 1600;
                    break;
                default:
                    pExposureValue->nSensitivity = 100;
                    break;
                }
            }

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_GetProperty(
                        cam_h, CAMCTRL_METERING_EXPOSURE, &metering_mode);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting metering mode");
            }
            DBGT_PTRACE("retrieved configuration metering mode: %d", metering_mode);
            pExposureValue->eMetering = (OMX_METERINGTYPE)metering_mode;
            break;
        }
    case OMX_IndexConfigCommonExposure:
        {
            OMX_CONFIG_EXPOSURECONTROLTYPE *pExposureControl = (OMX_CONFIG_EXPOSURECONTROLTYPE *) p;
            int exposure = 0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_EXPOSURE, &exposure);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting exposure");
            }
            DBGT_PTRACE("retrieved exposure configuration: %d", exposure);
            pExposureControl->eExposureControl = (OMX_EXPOSURECONTROLTYPE)exposure;
            break;
        }
    case OMX_IndexConfigCommonContrast:
        {
            OMX_CONFIG_CONTRASTTYPE *pContrast = (OMX_CONFIG_CONTRASTTYPE *) p;
            int contrast=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_CONTRAST, &contrast);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting contrast");
            }
            DBGT_PTRACE("retrieved contrast configuration: %d", contrast);
            pContrast->nContrast = contrast;
            break;
        }
    case OMX_Symbian_IndexConfigSharpness:
        {
            OMX_SYMBIAN_CONFIG_S32TYPE *pSharpness = (OMX_SYMBIAN_CONFIG_S32TYPE *) p;
            int sharpness=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(
                    cam_h, CAMCTRL_SHARPNESS, &sharpness);

            if(!bSuccess) {
                DBGT_WARNING("Error in getting sharpness");
            }
            DBGT_PTRACE("retrieved configuration: %d", sharpness);
            pSharpness->nValue = sharpness;
            break;
        }
    case OMX_IndexConfigCommonSaturation:
        {
            OMX_CONFIG_SATURATIONTYPE *pSaturation = (OMX_CONFIG_SATURATIONTYPE *) p;
            int saturation=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_SATURATION, &saturation);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting saturation");
            }
            DBGT_PTRACE("retrieved saturation configuration: %d", saturation);
            pSaturation->nSaturation = saturation;
            break;
        }
    case OMX_Symbian_IndexConfigFlickerRemoval:
        {
            OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE *pFlickerRemoval = (OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE *) p;
            int flickerremoval=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_FLICKER_MODE, &flickerremoval);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting flickerremoval");
            }
            DBGT_PTRACE("retrieved flickerremoval configuration: %d", flickerremoval);
            pFlickerRemoval->eFlickerRemoval = (OMX_SYMBIAN_FLICKERREMOVALTYPE)flickerremoval;
            break;
        }
    case OMX_IndexConfigCommonGamma:
        {
            OMX_CONFIG_GAMMATYPE * pGammaType = (OMX_CONFIG_GAMMATYPE*)p;
            int gamma=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_GAMMA, &gamma);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting gamma");
            }
            DBGT_PTRACE("retrieved gamma configuration: %d", gamma);
            pGammaType->nGamma = gamma-100;
            break;
        }
    case OMX_IndexConfigCommonImageFilter:
        {
            OMX_CONFIG_IMAGEFILTERTYPE * pImgFilter = (OMX_CONFIG_IMAGEFILTERTYPE *)p;

            int imagefilter = CAMCTRL_IMAGE_EFFECT_NONE;

            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_IMAGE_EFFECT, &imagefilter);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting imagefilter");
            }
            DBGT_PTRACE("retrieved imagefilter configuration: %d", imagefilter);

            switch(imagefilter){
            case CAMCTRL_IMAGE_EFFECT_GRAYSCALE:
                pImgFilter->eImageFilter = (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterGrayScale; break;
            case CAMCTRL_IMAGE_EFFECT_NEGATIVE:
                pImgFilter->eImageFilter = (OMX_IMAGEFILTERTYPE)OMX_ImageFilterNegative; break;
            case CAMCTRL_IMAGE_EFFECT_SOLARIZE:
                pImgFilter->eImageFilter = (OMX_IMAGEFILTERTYPE)OMX_ImageFilterSolarize; break;
            case CAMCTRL_IMAGE_EFFECT_SEPIA:
                pImgFilter->eImageFilter = (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterSepia; break;
            case CAMCTRL_IMAGE_EFFECT_POSTERIZE:
                pImgFilter->eImageFilter = (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterPosterize; break;
            case CAMCTRL_IMAGE_EFFECT_AQUA:
                pImgFilter->eImageFilter = (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterWatercolor; break;
            default:
                pImgFilter->eImageFilter = (OMX_IMAGEFILTERTYPE)OMX_ImageFilterNone; break;
            }
            break;
        }
    case OMX_Symbian_IndexConfigSceneMode:
        {
            OMX_SYMBIAN_CONFIG_SCENEMODETYPE * pSceneMode = (OMX_SYMBIAN_CONFIG_SCENEMODETYPE *)p;
            int scenemode=0;
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetProperty(cam_h, CAMCTRL_SCENE_MODE, &scenemode);
            if(!bSuccess) {
                DBGT_WARNING("Error in getting scenemode");
            }
            DBGT_PTRACE("retrieved scenemode configuration: %d", scenemode);

            switch(scenemode){
            case CAMCTRL_SCENE_MODE_PORTRAIT:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_ScenePortrait; break;
            case CAMCTRL_SCENE_MODE_LANDSCAPE:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneLandscape; break;
            case CAMCTRL_SCENE_MODE_NIGHT:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneNight; break;
            case CAMCTRL_SCENE_MODE_NIGHT_PORTRAIT:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneNightPortrait; break;
            case CAMCTRL_SCENE_MODE_SPORT:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneSport; break;
            case CAMCTRL_SCENE_MODE_BEACH:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Beach; break;
            case CAMCTRL_SCENE_MODE_PARTY:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Party; break;
            case CAMCTRL_SCENE_MODE_SNOW:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Snow; break;
            case CAMCTRL_SCENE_MODE_SUNSET:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Sunset; break;
            case CAMCTRL_SCENE_MODE_CANDLELIGHT:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Candlelight; break;
            default:
                pSceneMode->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneAuto; break;
            }
            break;
        }
    case IFM_IndexConfigCommonOpMode_Check:
        {
            IFM_CONFIG_OPMODE_CHECK * pOpMode = (IFM_CONFIG_OPMODE_CHECK *)p;
            pOpMode->eOpMode = iOpModeMgr.CurrentOperatingMode;
            break;
        }
    case OMX_Symbian_IndexConfigOrientationScene:
        {
            *(OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE*)p = ((COmxCamera*)&mENSComponent)->mSceneOrientation;
            break;
        }
    case OMX_STE_IndexConfigCommonLensParameters:
        {
            OMX_STE_CONFIG_LENSPARAMETERTYPE * pLensParam = (OMX_STE_CONFIG_LENSPARAMETERTYPE *)p;
            int horizontal_angle=0;
            int vertical_angle=0;
            int focal_length =0;

            int bSuccess = cam_Ctrllib->Camera_CtrlLib_Get_LensParameters(cam_h,&horizontal_angle,&vertical_angle, &focal_length );
            if(!bSuccess) {
                DBGT_WARNING("Error in getting LensParam");
            }

            pLensParam->nHorizontolViewAngle= horizontal_angle;
            pLensParam->nVerticalViewAngle= vertical_angle;
            pLensParam->nFocalLength = focal_length ;/* focal length as a multiple of 100 */
            DBGT_PTRACE("retrieved lens parameters horizontal_angle= %d, vertical_angle =%d focal length %d",
                 (int) pLensParam->nHorizontolViewAngle,
                 (int) pLensParam->nVerticalViewAngle,
                 (int) pLensParam->nFocalLength );

            break;
        }
    case OMX_Symbian_IndexConfigFlashControl:
        {
            DBGT_PTRACE("Get Flash Config");
            cam_Ctrllib->Camera_CtrlLib_GetAutoFlashThreshold(cam_h,
                    &auto_flash_on_threshold, &auto_flash_off_threshold );
            cam_Ctrllib->Camera_CtrlLib_GetFlashSynchroSupport(cam_h,
                    &synchro_flash_support );
            break;
        }
    case OMX_Symbian_IndexConfigExposureLock:
    case OMX_Symbian_IndexConfigWhiteBalanceLock:
        break;
    case IFM_IndexConfigCommonWritePe:
    case IFM_IndexConfigCommonReadPe:
    case OMX_IndexConfigCallbackRequest:
    case OMX_Symbian_IndexConfigMotionLevel:
    case OMX_Symbian_IndexConfigNDFilterControl:
    case OMX_Symbian_IndexConfigFocusLock:
    case OMX_Symbian_IndexConfigAllLock:
    case OMX_Symbian_IndexConfigROI:
    case OMX_IndexConfigCommonLightness:
    case OMX_IndexConfigCommonFocusRegion:
        {
            return OMX_ErrorUnsupportedSetting;
        }
    default:
        {
            err = mENSComponent.getConfig(aIdx,p);
            break;
        }
    }

    DBGT_EPILOG("");
    return err;
}

void *beginAutoFocusThread(void *cookie)
{
    Camera *c = (Camera *)cookie;
    c->autoFocusThread();
    return NULL;
}

void Camera::autoFocusThread( )
{
    int bSuccess ;
    int DelayTime;
    int af_status_reg = 0;
    int loop_count = 0;

    COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;

    DBGT_PROLOG("");

    pthread_mutex_lock(&mutex);

    bSuccess= cam_Ctrllib->Camera_CtrlLib_SetProperty(
            cam_h,
            CAMCTRL_FOCUS_MODES,
            OMXCam->mFocusControlType.eFocusControl,
            &DelayTime );
    if(!bSuccess) {
        DBGT_WARNING("Error in setting focus control type");
    }

    loop_count = 0;
    while (loop_count < FOCUS_STATUS_LOOP_MAX) {
        usleep(FOCUS_STATUS_WAIT*1000);

        if ( is_af_cancel ) {
            goto AF_STATUS_END;
        }

        int nFocusStatus=0;
        int bSuccess = cam_Ctrllib->Camera_CtrlLib_GetFocusStatus(
                cam_h, &nFocusStatus);
        if(!bSuccess) {
            DBGT_PTRACE("Focus not yet reached.");
        } else {
            DBGT_PTRACE("Focus reached.");
        }

        if ( (OMX_FOCUSSTATUSTYPE)nFocusStatus == OMX_FocusStatusReached )
            break;

        loop_count++;
    }

AF_STATUS_END:
    pthread_mutex_unlock(&mutex);

    if ((loop_count >= FOCUS_STATUS_LOOP_MAX) || (is_af_cancel == true))
    {
        DBGT_WARNING("Unable to reach focus or focus canceled");
        mENSComponent.eventHandlerCB(OMX_EventError, (OMX_U32)OMX_ErrorNone, OMX_FocusStatusUnableToReach, 0);
    } else {
        mENSComponent.eventHandlerCB(OMX_EventError, (OMX_U32)OMX_ErrorNone, OMX_FocusStatusReached, 0);
    }

    af_thread_t = 0;

    DBGT_EPILOG("");
}

/* */
/* Camera::applyConfig */
/* */
OMX_ERRORTYPE Camera::applyConfig(
        OMX_INDEXTYPE nIndex,
        OMX_PTR pStructure)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    int DelayTime = 0;
    int bSuccess = false;

    COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;

    if (pStructure == 0) {
        error = OMX_ErrorBadParameter;
        goto end;
    }

    switch ((uint32_t)nIndex) {
    case OMX_Symbian_IndexConfigCommonExtCapturing:
        {
            OMX_STATETYPE currentState = OMX_StateInvalid;
            mENSComponent.GetState(&mENSComponent, &currentState);

            if(currentState == OMX_StateExecuting) {
                s_scf_event * event = iDeferredEventMgr.getEmptyEvent();
                event->sig = CAMERA_EXIT_PREVIEW_SIG;
                event->type.capturingStruct = OMXCam->mCapturingStruct;
                iDeferredEventMgr.queueNewEvent(event);
                s_scf_event devent;
                devent.sig = Q_DEFERRED_EVENT_SIG;
                p_cam_sm->ProcessEvent(&devent);
            } else {
                t_uint16 portIndex=OMXCam->mCapturingStruct.nPortIndex;
                iOpModeMgr.ConfigCapturing[portIndex] = OMXCam->mCapturingStruct.bEnabled;
                if(portIndex==CAMERA_PORT_OUT1) {
                    DBGT_PTRACE("OMX_IndexConfigCapturing set to %d in Loaded or Idle state",
                            iOpModeMgr.ConfigCapturing[portIndex]);
                    DBGT_PTRACE("will be applied on executing state callback");
                }
            }
            break;
        }
    case OMX_IndexAutoPauseAfterCapture :
        {
            OMX_CONFIG_BOOLEANTYPE shouldAutoPauseOrNot = OMXCam->mAutoPauseAfterCapture;
            iOpModeMgr.AutoPauseAfterCapture = shouldAutoPauseOrNot.bEnabled;
            break;
        }
    case OMX_IndexConfigCaptureMode:
        {
            /* dynamic update */
            if (OMX_TRUE == iOpModeMgr.ConfigCapturing[CapturePort])
            {
                /* do not allow dynamic update of the number of frames of a limited burst */
                if (iOpModeMgr.IsBurstLimited() == (bool) OMXCam->mCaptureModeType.bFrameLimited)
                {
                    return OMX_ErrorNotReady;
                }

                iOpModeMgr.SetBurstLimited(OMXCam->mCaptureModeType.bFrameLimited == OMX_TRUE);
                iOpModeMgr.SetBurstFrameLimit(OMXCam->mCaptureModeType.nFrameLimit);

                /* user turns an infinite burst into a finite burst */
                /* FrameLimit is the number of frames to be streamed from now */
                if (iOpModeMgr.IsBurstLimited())
                {
                    iOpModeMgr.SetBurstFrameLimit(iOpModeMgr.GetBurstFrameLimit() + iOpModeMgr.UserBurstNbFrames);
                }
                iOpModeMgr.UpdateCurrentOpMode();
            }
            else
            {
                iOpModeMgr.SetBurstLimited(OMXCam->mCaptureModeType.bFrameLimited == OMX_TRUE);
                iOpModeMgr.SetBurstFrameLimit(OMXCam->mCaptureModeType.nFrameLimit);
            }

            break;
        }
    case OMX_Symbian_IndexConfigExtDigitalZoom :
        {
            int DelayTime = 0;
            camport * port = (camport *)  mENSComponent.getPort(CapturePort);
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(port->getPortIndex())->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

            cam_h->Registers[1]= (mParamPortDefinition.format.image.nFrameWidth >> 8) & (0XFF);
            cam_h->Registers[2]= (mParamPortDefinition.format.image.nFrameWidth ) & (0XFF);

            cam_h->Registers[3]= (mParamPortDefinition.format.image.nFrameHeight >> 8) & (0XFF);
            cam_h->Registers[4]= (mParamPortDefinition.format.image.nFrameHeight ) & (0XFF);
            int bSuccess = cam_Ctrllib->Camera_CtrlLib_SetDigitalZoom(
                    cam_h,
                    OMXCam->mZoomFactorCmdType.xZoomFactor.nValue ,
                    &DelayTime );

            if(!bSuccess) {
                DBGT_WARNING("Error in setting digital zoom %d",(int)OMXCam->mZoomFactorCmdType.xZoomFactor.nValue);
            }
            break;
        }
    case OMX_IndexConfigCommonWhiteBalance:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_WHITE_BALANCE,
                        OMXCam->mWhiteBalControlType.eWhiteBalControl,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting white balance");
            }
            break;
        }
    case OMX_IndexConfigCommonBrightness:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_BRIGHTNESS,
                        OMXCam->mBrightnessType.nBrightness,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting brightness");
            }
            break;
        }
    case OMX_IndexConfigCommonExposureValue:
        {
            CamCtrl_Iso iso;
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_EXPOSURE_COMPENSATION,
                        OMXCam->mExposureValueType.xEVCompensation,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting exposure compensation");
            }

            if (OMXCam->mExposureValueType.bAutoSensitivity == true){
                iso = CAMCTRL_ISO_AUTO;
            } else {
                switch(OMXCam->mExposureValueType.nSensitivity){
                case 100: iso = CAMCTRL_ISO_100; break;
                case 200: iso = CAMCTRL_ISO_200; break;
                case 400: iso = CAMCTRL_ISO_400; break;
                case 800: iso = CAMCTRL_ISO_800; break;
                case 1600: iso = CAMCTRL_ISO_1600; break;
                default: iso = CAMCTRL_ISO_100; break;
                }
            }

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_ISO,
                        iso,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting iso sensitivity");
            }

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_METERING_EXPOSURE,
                        OMXCam->mExposureValueType.eMetering,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting metering exposure");
            }
            break;
        }
    case OMX_IndexConfigCommonExposure:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_EXPOSURE,
                        OMXCam->mExposureControlType.eExposureControl,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting exposure");
            }
            break;
        }
    case OMX_IndexConfigCommonContrast:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_CONTRAST,
                        OMXCam->mContrastType.nContrast,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting contrast");
            }
            break;
        }
    case OMX_Symbian_IndexConfigSharpness:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h, CAMCTRL_SHARPNESS,
                        OMXCam->mSharpness.nValue,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting sharpness");
            }
            break;
        }
    case OMX_IndexConfigCommonSaturation:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_SATURATION,
                        OMXCam->mSaturationType.nSaturation,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting saturation");
            }
            break;
        }
    case OMX_IndexConfigCommonGamma:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_GAMMA,
                        OMXCam->mGammaType.nGamma,
                        &DelayTime);
            if(!bSuccess) {
                DBGT_WARNING("Error in setting gamma");
            }
            break;
        }
    case OMX_IndexConfigCommonImageFilter:
        {
            CamCtrl_ImageFilter imageFilter = CAMCTRL_IMAGE_EFFECT_NONE;

            switch((uint32_t)OMXCam->mImageFilterType.eImageFilter){
            case (OMX_IMAGEFILTERTYPE)OMX_ImageFilterNone:
                imageFilter = CAMCTRL_IMAGE_EFFECT_NONE; break;
            case (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterGrayScale:
                imageFilter = CAMCTRL_IMAGE_EFFECT_GRAYSCALE; break;
            case (OMX_IMAGEFILTERTYPE)OMX_ImageFilterNegative:
                imageFilter = CAMCTRL_IMAGE_EFFECT_NEGATIVE; break;
            case (OMX_IMAGEFILTERTYPE)OMX_ImageFilterSolarize:
                imageFilter = CAMCTRL_IMAGE_EFFECT_SOLARIZE; break;
            case (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterSepia:
                imageFilter = CAMCTRL_IMAGE_EFFECT_SEPIA; break;
            case (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterPosterize:
                imageFilter = CAMCTRL_IMAGE_EFFECT_POSTERIZE; break;
            case (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterWatercolor:
                imageFilter = CAMCTRL_IMAGE_EFFECT_AQUA; break;
            default:
                imageFilter = CAMCTRL_IMAGE_EFFECT_NONE; break;
            }

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_IMAGE_EFFECT,
                        imageFilter,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting image filter");
            }
            break;
        }
    case OMX_IndexConfigFocusControl:
        {
            OMX_IMAGE_FOCUSCONTROLTYPE focusControlType= ((OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE*)pStructure)->eFocusControl;
            if ( focusControlType == OMX_IMAGE_FocusControlOff ) {
                DBGT_PTRACE("OMX_IndexConfigFocusControl cancel");
                void* t_return;
                is_af_cancel = true;
            } else if ( focusControlType == OMX_IMAGE_FocusControlAuto ) {
                bSuccess= cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_FOCUS_MODES,
                        OMXCam->mFocusControlType.eFocusControl,
                        &DelayTime );
                if(!bSuccess) {
                    DBGT_WARNING("Error in setting focus control type");
                }
            } else {
                is_af_cancel = false;
                pthread_create( &af_thread_t, NULL,  beginAutoFocusThread, (void*)this);
            }
            break;
        }
    case OMX_Symbian_IndexConfigFocusRange:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_FOCUS_RANGE,
                        OMXCam->mFocusRangeType.eFocusRange,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting focus range type");
            }            break;
        }
    case OMX_Symbian_IndexConfigSceneMode:
        {
            CamCtrl_SceneMode eSceneType = CAMCTRL_SCENE_MODE_AUTO;

            switch((uint32_t)OMXCam->mSceneMode.eSceneType){
            case OMX_SYMBIAN_ScenePortrait:
                eSceneType = CAMCTRL_SCENE_MODE_PORTRAIT; break;
            case OMX_SYMBIAN_SceneLandscape:
                eSceneType = CAMCTRL_SCENE_MODE_LANDSCAPE; break;
            case OMX_SYMBIAN_SceneNight:
                eSceneType = CAMCTRL_SCENE_MODE_NIGHT; break;
            case OMX_SYMBIAN_SceneNightPortrait:
                eSceneType = CAMCTRL_SCENE_MODE_NIGHT_PORTRAIT; break;
            case OMX_SYMBIAN_SceneSport:
                eSceneType = CAMCTRL_SCENE_MODE_SPORT; break;
            case OMX_STE_Scene_Beach:
                eSceneType = CAMCTRL_SCENE_MODE_BEACH; break;
            case OMX_STE_Scene_Party:
                eSceneType = CAMCTRL_SCENE_MODE_PARTY; break;
            case OMX_STE_Scene_Snow:
                eSceneType = CAMCTRL_SCENE_MODE_SNOW; break;
            case OMX_STE_Scene_Sunset:
                eSceneType = CAMCTRL_SCENE_MODE_SUNSET; break;
            case OMX_STE_Scene_Candlelight:
                eSceneType = CAMCTRL_SCENE_MODE_CANDLELIGHT; break;
            default:
                eSceneType = CAMCTRL_SCENE_MODE_AUTO; break;
            }

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h,
                        CAMCTRL_SCENE_MODE,
                        OMXCam->mSceneMode.eSceneType,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting scene mode");
            }
            break;
        }
    case OMX_Symbian_IndexConfigFlickerRemoval:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h, CAMCTRL_FLICKER_MODE,
                        OMXCam->mFlickerRemoval.eFlickerRemoval,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting flicker cancellation");
            }
            break;
        }
    case OMX_Symbian_IndexConfigOrientationScene:
        /* Noting to do here */
        return OMX_ErrorNone;
    break;
    case OMX_IndexConfigCommonFrameStabilisation:
        {
            iGrabControl.mStabEnabled = OMXCam->mStabType.bStab;
            break;
        }
    case OMX_Symbian_IndexConfigExposureLock:
        {
          if(OMXCam->mExposureLock.eImageLock==OMX_SYMBIAN_LockOff)
            {
               DBGT_PTRACE("Apply Config Remove the ExposureLock ");
                bSuccess =
               cam_Ctrllib->Camera_CtrlLib_ExposureLock(
                    cam_h,
                        &DelayTime );
                if(!bSuccess) {
                DBGT_WARNING("Error in Setting Exposure Lock");
                }
              }
            else if(OMXCam->mExposureLock.eImageLock==OMX_SYMBIAN_LockImmediate)
            {
                DBGT_PTRACE("Apply Config do the ExposureLock ");
                 bSuccess =
               cam_Ctrllib->Camera_CtrlLib_ExposureUnLock(
                       cam_h,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in Setting Exposure Lock");
            }
            }
            break;
        }
    case OMX_Symbian_IndexConfigWhiteBalanceLock:
       {
        if(OMXCam->mWhiteBalanceLock.eImageLock==OMX_SYMBIAN_LockOff)
            {
               DBGT_PTRACE("Apply Config Remove the AWBLock ");
                bSuccess =
         cam_Ctrllib->Camera_CtrlLib_AwbLock(
                      cam_h,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in Setting AWB Lock");
            }
            }
            else if(OMXCam->mWhiteBalanceLock.eImageLock==OMX_SYMBIAN_LockImmediate)
            {
                DBGT_PTRACE("Apply Config Do the AWBLock ");
                  bSuccess =
               cam_Ctrllib->Camera_CtrlLib_AwbUnLock(
                       cam_h,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in Setting CtrlLib_AwbUnLock");
            }
            }
        }
        break;
    case OMX_IndexParamQFactor:
        {
            int grade;
            OMX_IMAGE_PARAM_QFACTORTYPE *jpgQfactor =  (OMX_IMAGE_PARAM_QFACTORTYPE*)pStructure;
            OMX_U32  quality =  jpgQfactor->nQFactor >> 16;
            if ( quality >= 80) {
                grade = 0;
            } else if (  quality >= 50  ) {
                grade = 1;
            } else {
                grade = 2;
            }

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty( cam_h,
                    CAMCTRL_JPEG_COMPRESSION_LEVEL,
                     grade,  &DelayTime );
            if(!bSuccess) {
                DBGT_CRITICAL("OMX_IndexParamQFactor");
            }
            break;
        }
    case OMX_Symbian_IndexConfigFocusRegion:
        {
            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_GET_MAX_TOUCH_X_Y_VALUES(
                        cam_h);
            if(!bSuccess) {
                DBGT_WARNING("Error in setting Touch focus region ");
            }

            DBGT_PTRACE("Max X and Y co-ordiantes supported by sensor %d %d",cam_h->Registers[0],cam_h->Registers[1]);
            OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nX=(cam_h->Registers[0]* OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nX)/OMXCam->mFocusRegionType.sFocusRegion.sReference.nWidth;
            OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nY=(cam_h->Registers[1]* OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nY)/OMXCam->mFocusRegionType.sFocusRegion.sReference.nHeight;
            DBGT_PTRACE("Mapping nX =%d nY =%d nWidth =%d nHeight =%d",
                                   (int)OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nX,
                                   (int)OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nY,
                                   (int)OMXCam->mFocusRegionType.sFocusRegion.sReference.nWidth,
                                   (int)OMXCam->mFocusRegionType.sFocusRegion.sReference.nHeight) ;

            cam_h->Registers[0]=OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nX;
            cam_h->Registers[1]=OMXCam->mFocusRegionType.sFocusRegion.sRect.sTopLeft.nY;

            bSuccess =
                cam_Ctrllib->Camera_CtrlLib_SetProperty(
                        cam_h, CAMCTRL_TOUCHAF_REGION,
                        0,
                        &DelayTime );
            if(!bSuccess) {
                DBGT_WARNING("Error in setting Touch focus region ");
            }


        }
        break;
    case OMX_Symbian_IndexConfigFlashControl:
        {
        TFlashMode flashMode=FLASH_MODE_STILL_LED_EXTERNAL_STROBE;
        m_auto_flash_on=0;
        m_is_need_flash_on=0;

        if (!synchro_flash_support)
            flashMode=FLASH_MODE_STILL_LED;

        if (ExtCamSlot == ePrimaryExtCamera) {
            if ( OMXCam->mFlashControlStruct.eFlashControl == OMX_IMAGE_FlashControlTorch ) {
            cameraFlashOnOff(CAMERA_FLASH_ON, FLASH_MODE_VIDEO_LED);
            }
            else if ( OMXCam->mFlashControlStruct.eFlashControl == OMX_IMAGE_FlashControlOff ) {
                cam_Ctrllib->Camera_CtrlLib_StopFlash(
                cam_h,
                &DelayTime );
                // switch off torch
            }
            else if( OMXCam->mFlashControlStruct.eFlashControl == OMX_IMAGE_FlashControlAuto ) {
                cam_Ctrllib->Camera_CtrlLib_StopFlash(
                cam_h,
                &DelayTime);

                cameraFlashOnOff(FLASH_OFF, flashMode);
                cameraFlashOnOff(CAMERA_FLASH_ON, flashMode);
                m_auto_flash_on=1;
            }
           else if( OMXCam->mFlashControlStruct.eFlashControl == OMX_IMAGE_FlashControlOn ) {
                cam_Ctrllib->Camera_CtrlLib_StopFlash(
                cam_h,
                &DelayTime);
                cameraFlashOnOff(FLASH_OFF, flashMode);
                cameraFlashOnOff(CAMERA_FLASH_ON, flashMode);
                m_is_need_flash_on=1;
            }
        }
        }
        break;
    case OMX_IndexConfigCommonColorBlend:
    case OMX_Symbian_IndexConfigCenterFieldOfView:
    case OMX_IndexConfigCommonLightness:
    case OMX_IndexConfigCommonFocusRegion:
    case OMX_Symbian_IndexConfigMotionLevel:
    case OMX_Symbian_IndexConfigFocusLock:
    case OMX_Symbian_IndexConfigAllLock:
    case OMX_Symbian_IndexConfigROI:
    case OMX_IndexConfigCallbackRequest:
    case OMX_Symbian_IndexConfigRAWPreset:
    case IFM_IndexConfigCommonReadPe:
    case IFM_IndexConfigCommonWritePe:
    case IFM_IndexConfigTestMode:
    case OMX_IndexConfigCommonMirror:
        {
            return OMX_ErrorUnsupportedSetting;
        }

#ifdef ANDROID
    case OMX_google_android_index_enableAndroidNativeBuffers:
    case OMX_google_android_index_useAndroidNativeBuffer:
        break;
#endif

    default:
        {
            error = OMX_ErrorUnsupportedIndex;
            goto end;
        }
    }

end:
    DBGT_EPILOG("");
    return error;
}


/* */
/* Camera::instantiate */
/* */
OMX_ERRORTYPE Camera::instantiate()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE error = OMX_ErrorNone;
    t_nmf_error nmf_err = NMF_OK;

    OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();

    // Instantiate and bind EXT_ISPCTL NMF Component
    iExtIspctlComponentManager.setOMXHandle(handle);
    error = iExtIspctlComponentManager.instantiateNMF();
    if (error != OMX_ErrorNone){
        DBGT_PTRACE("Error: iExtIspctlComponentManager.instantiateNMF() 0x%X", error);
        goto end;
    }

    // Instantiate ImgNetwork
    mpImgNetwork = ext_img_networkCreate();

    mpImgNetwork->domain = mENSComponent.getNMFDomainHandle();
    nmf_err = mpImgNetwork->construct();
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: mpImgNetwork->construct() 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }
    else
    {
        DBGT_PTRACE("Construct Passed");
    }

    // bind provided interfaces
    nmf_err = mpImgNetwork->bindFromUser(
            "sendcommand",
            1,
            &mIsendCommand);
    DBGT_PTRACE("Bind sendcommand nmf_err = %d",nmf_err);
    nmf_err |= mpImgNetwork->bindFromUser(
            "fsminit",
            1,
            &mIfsmInit);
    DBGT_PTRACE("Bind fsminit nmf_err = %d",nmf_err);
    nmf_err |= mpImgNetwork->bindFromUser(
            "fillthisbuffer[0]",
            mENSComponent.getPort(CAMERA_PORT_OUT0)->getBufferCountActual(),
            &mIfillThisBuffer[0]);
    DBGT_PTRACE("Bind fillthisbuffer[0] nmf_err = %d",nmf_err);
    nmf_err |= mpImgNetwork->bindFromUser(
            "fillthisbuffer[1]",
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getBufferCountActual(),
            &mIfillThisBuffer[1]);
    DBGT_PTRACE("Bind fillthisbuffer[1] nmf_err = %d",nmf_err);
    nmf_err |= mpImgNetwork->bindFromUser(
            "grabctl_configure",
            1,
            &mIgrabctlConfigure);
    DBGT_PTRACE("Bind grabctl_configure nmf_err = %d",nmf_err);
    nmf_err |= mpImgNetwork->bindFromUser(
            "stab_configure",
            3,
            &mIstabConfigure);
    DBGT_PTRACE("Bind stab_configure nmf_err = %d",nmf_err);
    nmf_err |= mpImgNetwork->bindFromUser(
            "grab_command",
            1,
            &iGrab.mGrabCmd);
    DBGT_PTRACE("Bind grab_command nmf_err = %d",nmf_err);
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: mpImgNetwork->bindFromUser() 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }

    // bind callbacks
    nmf_err |= EnsWrapper_bindToUser(
            handle,
            mpImgNetwork,
            "proxy",
            this->getEventHandlerCB(),
            3);
    nmf_err |= EnsWrapper_bindToUser(
            handle,
            mpImgNetwork,
            "fillbufferdone[0]",
            &iCallback,
            mENSComponent.getPort(CAMERA_PORT_OUT0)->getBufferCountActual());
    nmf_err |= EnsWrapper_bindToUser(
            handle,
            mpImgNetwork,
            "fillbufferdone[1]",
            &iCallback,
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getBufferCountActual());
    nmf_err |= EnsWrapper_bindToUser(
            handle,
            mpImgNetwork,
            "grabctl_acknowledges",
            (ext_grabctl_api_acknowledgesDescriptor *)this,
            4);
    nmf_err |= EnsWrapper_bindToUser(
            handle,
            mpImgNetwork,
            "grabctl_error",
            (ext_grabctl_api_errorDescriptor *)this,
            16);
    nmf_err |= EnsWrapper_bindToUser(
            handle,
            mpImgNetwork,
            "grab_alert",
            (ext_grab_api_alertDescriptor*)this,
            4);
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: mpImgNetwork->EnsWrapper_bindToUser() 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }

    // Instantiate Timer
    pTimer = timerWrappedCreate();

    nmf_err = pTimer->construct();
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: pTimer->construct() %d", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }

    nmf_err = pTimer->getInterface("timer", &mTimer);
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: pTimer->bindFromUser() %d", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }

    nmf_err = EnsWrapper_bindToUser(handle, pTimer, "alarm", (timer_api_alarmDescriptor*)this, 4);
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: pTimer->EnsWrapper_bindToUser() %d", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }

    /* +ER363747 */
    instantiateDone = OMX_TRUE;
    /* -ER363747 */

end:
    DBGT_EPILOG("");
    return error;
}


/* */
/* Camera::start */
/* */
OMX_ERRORTYPE Camera::start()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE omxerr = OMX_ErrorNone;

    // starts EXT_ISPCTL NMF component and loads ISP FW
    omxerr = iExtIspctlComponentManager.startNMF();
    if (omxerr != OMX_ErrorNone)
        goto end;

    // starts ImgNetwork NMF Component
    mpImgNetwork->start();

    // starts timer NMF Component
    pTimer->start();

    // sets the command IF from EXT_ISPCTL NMF to the ExtIspctlCom object
    iExtIspctlCom.setExtIspctlCmd(iExtIspctlComponentManager.getExtIspctlCmd());

    // sets the mIgrabctlConfigure IF from ImgNetwork to the GrabControl object
    iGrabControl.setConfigureControl(mIgrabctlConfigure);

    // sets the mIstabConfigure IF from ImgNetwork to the GrabControl object
    iGrabControl.setStabConfigure(mIstabConfigure);

    // iSiaPlatformManager uses the ext_ispctl configure interface so it is passed from iExtIspctlComponentManager to iSiaPlatformManager
    iSiaPlatformManager.setExtIspctlCfg(iExtIspctlComponentManager.getExtIspctlCfg());

    // configure the port/pipe mapping
    mIgrabctlConfigure.setPort2PipeMapping(GRBPID_PIPE_CAM,GRBPID_PIPE_CAM,GRBPID_PIPE_CAM);

    // tell DSP the CCP/CSI config
    iSiaPlatformManager.config(ExtCamSlot);

    //Enable DPHY
    configureIPP(HW_DPHY_ENABLE);

    /* +ER363747 */
    startDone = OMX_TRUE;
    /* -ER363747 */

end:
    DBGT_EPILOG("");
    return omxerr;
}


/* */
/* Camera::stop */
/* */
OMX_ERRORTYPE Camera::stop()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE omxerr = OMX_ErrorNone;

    void* t_return;

    if ( af_thread_t )
        pthread_join( af_thread_t , &t_return);

    // Disable DPHY
    configureIPP(HW_DPHY_DISABLE);

    // Deinit Camera
    int DelayTime;
    cam_Ctrllib->Camera_CtrlLib_Deinitialize(cam_h, &DelayTime);

    // Stop EXT_ISPCTL
    omxerr = iExtIspctlComponentManager.stopNMF();
    if(omxerr != OMX_ErrorNone)
        goto end;

    // Stop ImgNetwork
    mpImgNetwork->stop();

    // Stop itmer
    pTimer->stop();

    /* +ER363747 */
    startDone = OMX_FALSE;
    /* -ER363747 */

end:
    DBGT_EPILOG("");
    return omxerr;
}


/* */
/* Camera::configure */
/* */
OMX_ERRORTYPE Camera::configure() {
    OMX_ERRORTYPE omxerr = OMX_ErrorNone;
    return omxerr;
}



/* */
/* Camera::doSpecificSendCommand is used to delay the SendCommand */
/* */
OMX_ERRORTYPE Camera::doSpecificSendCommand(
        OMX_COMMANDTYPE cmd,
        OMX_U32 nParam,
        OMX_BOOL &bDeferredCmd)
{
    DBGT_PROLOG("");

    OMX_STATETYPE currentState = OMX_StateInvalid;
    bDeferredCmd = OMX_FALSE;
    mENSComponent.GetState(&mENSComponent, &currentState);

    if (cmd == OMX_CommandFlush) {
        p_cam_sm->flushRequest = true;

        if (p_cam_sm->watchdogCounter > 0) {
            /* Force to dequeue buffer located at MPC side
             * if watchdog timeout already occured */
            DBGT_PTRACE("Watchdog timeout occurs then flush buffer requested => empty the buffer located at MPC side.");
            p_cam_sm->pGrabControl->setForceDequeueBuffer(CAMERA_PORT_OUT0);
        }
    }

    if(  ((cmd == OMX_CommandStateSet)&&(nParam==OMX_StateIdle)&&(currentState == OMX_StateExecuting))
            ||((cmd == OMX_CommandPortEnable)&&((currentState == OMX_StateExecuting)||(currentState == OMX_StateIdle)))
            ||((cmd == OMX_CommandPortDisable)&&(currentState == OMX_StateExecuting))
      ) {
        bDeferredCmd = OMX_TRUE;

        s_scf_event * event = iDeferredEventMgr.getEmptyEvent();

        if(cmd == OMX_CommandPortEnable) {
            camport * port = (camport *) mENSComponent.getPort(nParam); //in such case, nParam is the portIndex

            if(port->portSettingsChanged == OMX_TRUE) {
                event->sig = ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG;
            }
        } else {
            /* no new settings to be applied */
            event->sig = SEND_COMMAND_SIG;
        }

        event->args.sendCommand.Cmd =cmd;
        event->args.sendCommand.nParam = nParam;
        iDeferredEventMgr.queueNewEvent(event);
        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_cam_sm->ProcessEvent(&devent);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}


/* */
/* Camera::deInstantiate */
/* */
OMX_ERRORTYPE Camera::deInstantiate()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE error = OMX_ErrorNone;
    t_nmf_error nmf_err = NMF_OK;

    OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();

    /* free remaining buffers (doing it in destructor is too late for RME) */
    error = iGrabControl.freeGrabCache();
    if (error != OMX_ErrorNone) {
        DBGT_PTRACE("Error: iGrabControl.freeGrabCache() 0x%X", error);
        goto end;
    }

    // deinstantiate EXT_ISPCTL
    error = iExtIspctlComponentManager.deinstantiateNMF();
    if (error != OMX_ErrorNone){
        DBGT_PTRACE("Error: iExtIspctlComponentManager.deinstantiateNMF() 0x%X", error);
        goto end;
     }

    // deinstantiate NMF Network
    nmf_err |= mpImgNetwork->unbindFromUser("sendcommand");
    nmf_err |= mpImgNetwork->unbindFromUser("fsminit");
    nmf_err |= mpImgNetwork->unbindFromUser("fillthisbuffer[0]");
    nmf_err |= mpImgNetwork->unbindFromUser("fillthisbuffer[1]");
    nmf_err |= mpImgNetwork->unbindFromUser("grabctl_configure");
    nmf_err |= mpImgNetwork->unbindFromUser("grab_command");
    nmf_err |= mpImgNetwork->unbindFromUser("stab_configure");
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: img_network unbindfromUser 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }

    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "proxy");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[0]");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[1]");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_acknowledges");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_error");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "grab_alert");
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: img_network unbindToUser 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }
    nmf_err = mpImgNetwork->destroy();
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: mImgNetwork->destroy() 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }
    ext_img_networkDestroy(mpImgNetwork);


    // deinstantiate timer
    nmf_err = EnsWrapper_unbindToUser(handle, pTimer, "alarm");
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: pTimer->unbindToUser 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }
    nmf_err = pTimer->destroy();
    if (nmf_err != NMF_OK) {
        DBGT_PTRACE("Error: pTimer->destroy() 0x%X", nmf_err);
        error = OMX_ErrorHardware;
        goto end;
    }
    timerWrappedDestroy(pTimer);

    // exit from HSM Framework
    s_scf_event devent;
    devent.sig = Q_FINAL_SIG;
    p_cam_sm->ProcessEvent(&devent);

    destroySM();

    /* +ER363747 */
    instantiateDone = OMX_FALSE;
    /* -ER363747 */

end:
    DBGT_EPILOG("");
    return error;
}


/*
   called from COmxCamera::NmfPanicCallback in case of NMF panic on SIA.
   Brutally destroy the whole NMF network on DSP side
   Note that in this function we FORCE all components to stop and destroy
   whatever the error code returned by NMF functions !
   */
void Camera::errorRecovery()
{
    DBGT_WARNING("NMF panic happens! OMX.ST.ExtHSMCamera Entering error recovery ---- destroying NMF network");

    OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();

    iPanic = OMX_TRUE;

    /* For the case that something else is causing NMF panic, such as admserver. */
    if (p_cam_sm == NULL) {
        DBGT_WARNING("NMF panic happens! OMX.ST.ExtHSMCamera error recovery ---- p_cam_sm == NULL, aborting error recovery! \n");
        return;
    }

    /* Stop watchdog timer */
    p_cam_sm->watchdogStop();

    mpImgNetwork->stop();

    /* stop & deinstantiate ISPCTL */
    iExtIspctlComponentManager.forcedStopNMF();
    iExtIspctlComponentManager.deinstantiateNMF();

    /* deinstantiate NMF Network*/
    mpImgNetwork->unbindFromUser("sendcommand");
    mpImgNetwork->unbindFromUser("fsminit");
    mpImgNetwork->unbindFromUser("fillthisbuffer[0]");
    mpImgNetwork->unbindFromUser("fillthisbuffer[1]");
    mpImgNetwork->unbindFromUser("grabctl_configure");
    mpImgNetwork->unbindFromUser("grab_command");
    mpImgNetwork->unbindFromUser("stab_configure");

    EnsWrapper_unbindToUser(handle, mpImgNetwork, "proxy");
    EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[0]");
    EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[1]");
    EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_acknowledges");
    EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_error");
    EnsWrapper_unbindToUser(handle, mpImgNetwork, "grab_alert");

    mpImgNetwork->destroy();
    ext_img_networkDestroy(mpImgNetwork);

    iExtIspctlCom.setPanic(OMX_TRUE);

    DBGT_WARNING("NMF panic happens! OMX.ST.ExtHSMCamera error recovery ---- NMF network destroyed");
}

void Camera::cameraFlashOnOff( int enable, TFlashMode flashMode)
{
    bool switchOn=false;
    int driver_err;
    TFlashMode aModes;

    DBGT_PROLOG("");

    iFlashDriver->GetSupportedFlashModes(
        aModes,
        EPrimary);
    if (aModes==0){
        DBGT_WARNING("Error: No Supported flash mode.\n");
        return;
    }

    if (!(flashMode && aModes)){
        DBGT_WARNING("Error: Requested flash mode not supported.\n");
        return;
    }

    if (enable){
        switchOn = true;
        driver_err = iFlashDriver->EnableFlashMode(flashMode, NULL, NULL, EPrimary);
        if (driver_err != FLASH_RET_NONE) {
            DBGT_WARNING("Error: Cannot enable flash Hw driver.\n");
            return;
        }

        if (flashMode && FLASH_MODE_STILL_LED_EXTERNAL_STROBE ) {
            //  Duration according to RB code is 0
            driver_err = iFlashDriver->ConfigureFlashMode(flashMode, 0, 100, 0, EPrimary);
            if (driver_err != FLASH_RET_NONE) {
                DBGT_WARNING("Error: Cannot configure flash mode : 0x%x.\n", flashMode);
                return;
            }
        else if (flashMode && FLASH_MODE_STILL_LED ) {
            driver_err = iFlashDriver->ConfigureFlashMode(flashMode, 0, 100, 0, EPrimary);
            if (driver_err != FLASH_RET_NONE) {
                DBGT_WARNING("Error: Cannot configure flash mode : 0x%x.\n", flashMode);
                return;
            }}
        }
    }

    driver_err = iFlashDriver->Strobe(flashMode, switchOn, EPrimary);

    if (!enable){
        iFlashDriver->EnableFlashMode(FLASH_MODE_NONE, NULL, NULL, EPrimary);
    }

    if (driver_err != FLASH_RET_NONE) {
        DBGT_WARNING("Error: Manual strobe failed.\n");
    }

    DBGT_EPILOG("");
}

void Camera::printPrivateContext(OMX_OTHER_PARAM_PRIVATE_CONTEXT* pCtx)
{
    OMX_PTR pPvt = pCtx->pPrintPrivate;

    //Camera infos Section
    t_uint8* Name_p = new t_uint8[50];
    t_uint8* Vendor_p = new t_uint8[50];
    t_uint8* MajorVersion_p = new t_uint8[10];
    t_uint8* MinorVersion_p = new t_uint8[10];
    t_uint8* CameraOrientation_p = new t_uint8[10];
    char driverFileLocation[PROPERTY_VALUE_MAX];

    if (ExtCamSlot == ePrimaryExtCamera)
        property_get("ste.cam.ext.cfg.path.primary", driverFileLocation, "");
    else
        property_get("ste.cam.ext.cfg.path.secondary", driverFileLocation, "");

    cam_Ctrllib->Camera_CtrlLib_GetDriverInfo(
            cam_h,
            50,
            50,
            Name_p,
            Vendor_p,
            MajorVersion_p,
            MinorVersion_p,
            CameraOrientation_p);

    pCtx->pPrint(pPvt, "|-[Camera infos]\n");
    pCtx->pPrint(pPvt, "| |- Name : %s\n",
            Name_p);
    pCtx->pPrint(pPvt, "| |- Vendor : %s\n",
            Vendor_p);
    pCtx->pPrint(pPvt, "| |- Version (Major.Minor) : %d.%d\n",
            *MajorVersion_p,
            *MinorVersion_p);
    pCtx->pPrint(pPvt, "| |- Driver file location: %s\n",
            driverFileLocation);

    delete Name_p;
    delete Vendor_p;
    delete MajorVersion_p;
    delete MinorVersion_p;
    delete CameraOrientation_p;

    //Interface configuration section
    CamCtrl_CsiIntf_Config_t csiConfig;
    t_uint8 selCam      = CAMCTRL_CAM_SEL_SECONDARY;
    t_uint8 selDataIntf = CAMCTRL_CAM_DATA_INTF_CSI2;
    t_uint8 numOfLanes  = 0;

    cam_Ctrllib->Camera_CtrlLib_Get_cam_interface_config(cam_h,
            &selCam,
            &selDataIntf,
            &numOfLanes,
            &csiConfig);

    pCtx->pPrint(pPvt, "|-[Interface configuration]\n");
    if(CAMCTRL_CAM_DATA_INTF_CCP2 == selDataIntf) {
        pCtx->pPrint(pPvt, "| |- CCP2 interface\n");
        pCtx->pPrint(pPvt, "| |- Number of data lanes = %d\n", numOfLanes);
    } else {
        pCtx->pPrint(pPvt, "| |- CSI2 interface\n");
        pCtx->pPrint(pPvt, "| |- Number of data lanes = %d\n", numOfLanes);
        pCtx->pPrint(pPvt, "| |- [Dphy clock lane configuration]\n");
        if(csiConfig.Mipi_Specs_90_81b == 0x1) {
            pCtx->pPrint(pPvt, "| | |- Cammera follows v0.90 or v1.00 DPHY timing\n");
        } else {
            pCtx->pPrint(pPvt, "| | |- Cammera follows v0.81 DPHY timing\n");
        }
        pCtx->pPrint(pPvt, "| | |- CSI clock lane bitrate = %d Mbps / data lane\n", csiConfig.CSI_bitrate_mbps);
        pCtx->pPrint(pPvt, "| |- [Dphy data lane configuration]\n");
        if(csiConfig.Eot_Bypass == 0x1) {
            pCtx->pPrint(pPvt, "| | |- EOT bypass is enabled\n");
        } else {
            pCtx->pPrint(pPvt, "| | |- EOT bypass is disabled\n");
        }
        pCtx->pPrint(pPvt, "| | |- 1st data type understood by SIA = 0x%X (pixel width = %d)\n", csiConfig.CSI_DataType0, csiConfig.CSI_DataType0_PixWidth);
        pCtx->pPrint(pPvt, "| | |- 2nd data type understood by SIA = 0x%X (pixel width = %d)\n", csiConfig.CSI_DataType1, csiConfig.CSI_DataType1_PixWidth);
        pCtx->pPrint(pPvt, "| | |- 3rd data type understood by SIA = 0x%X (pixel width = %d)\n", csiConfig.CSI_DataType2, csiConfig.CSI_DataType2_PixWidth);
    }

    //Streaming information section
    pCtx->pPrint(pPvt, "|-[Streaming infos]\n");
    pCtx->pPrint(pPvt, "| |- Watchdog timeout is equal to %d ms\n",
            p_cam_sm->watchdogIntervalMsec);
    pCtx->pPrint(pPvt, "| |- Number of frame to skip before preview starts in still picture mode = %d\n",
            cam_Ctrllib->Camera_CtrlLib_GetVfFrameSkip(cam_h));
    pCtx->pPrint(pPvt, "| |- Number of frame to skip before preview starts in camcording mode = %d\n",
            cam_Ctrllib->Camera_CtrlLib_GetCcFrameSkip(cam_h));
    pCtx->pPrint(pPvt, "| |- Number of frame to skip before taking a picture = %d\n",
            cam_Ctrllib->Camera_CtrlLib_GetSsFrameSkip(cam_h));

    //Performances information section
    pCtx->pPrint(pPvt, "|-[Performances infos]\n");
    pCtx->pPrint(pPvt, "| |- Min framerate = %f fps\n",
            mMinFps);
    pCtx->pPrint(pPvt, "| |- Max framerate = %f fps\n",
            mMaxFps);
    pCtx->pPrint(pPvt, "| |- Average framerate = %f fps\n",
            mAvgFps);

    //Statistical information section
    pCtx->pPrint(pPvt, "|-[Statistical infos]\n");
    pCtx->pPrint(pPvt, "| |- Number of buffer ready to be filled for VPB0 port = %d buffer (at MPC side)\n",
            iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT0]);
    pCtx->pPrint(pPvt, "| |- Number of buffer ready to be filled for VPB1 port = %d buffer (at MPC side)\n",
            iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT1]);
    pCtx->pPrint(pPvt, "| |- Camera has been in startation %d time(s) on VPB0",
            mStarvationCnt);
    if (iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT0] == 0)
        pCtx->pPrint(pPvt, " and is currently in starvation on VPB0\n");
    else
        pCtx->pPrint(pPvt, "\n");

}

void Camera::performances(OMX_BUFFERHEADERTYPE* pBuffer)
{
    /* mCurrFrameTime in µs */
    mCurrFrameTime = pBuffer->nTimeStamp / 1000;

    /* Calculate current frame interval µs */
    if (mPrevFrameTime > 0)
        mFrameInterval = mCurrFrameTime - mPrevFrameTime;
    else
        mFrameInterval = 0;

    /* Calculate instant framerate */
    if (mFrameInterval > 0)
        mInstantFps = (float)(1000000.0 / mFrameInterval);
    else
        mInstantFps = 0;

    /* Calculate min / max framerate */
    if (mMinFps > 0)
        mMinFps = MIN(mInstantFps, mMinFps);
    else
        mMinFps = mInstantFps;

    if (mMaxFps > 0)
        mMaxFps = MAX(mInstantFps, mMaxFps);
    else
        mMaxFps = mInstantFps;

    /* Calculate avarage framerate */
    mTotalFrameInterval += mFrameInterval;
    mFrameCount++;

    if (mFrameCount > 0)
        mAvgFps = (float)(1000000.0 / (mTotalFrameInterval / mFrameCount));
    else
        mAvgFps = 0;

    mPrevFrameTime = mCurrFrameTime;
}

