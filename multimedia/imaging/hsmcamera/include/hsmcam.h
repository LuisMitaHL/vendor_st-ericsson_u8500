/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _CAM_SM_H_
#define _CAM_SM_H_

#include "hsm.h"
#include "osi_trace.h"

#include "zoom.h"
#include "test_mode.h"
#include "opmodemgr.h"
#include "tuning.h"
#include "tuning_data_base.h"
#include "tuning_loader_manager.h"
#include "sensor.h"
#include "grabctl.h"
#include "resource_sharer_manager.h"
#include "sw3A_component_manager.h"
#include "grab.h"
#include "selftest.h"
#include "capture_context.h"
#include "flash_api.h"
#include "cam_shared_memory.h"
#include "isp_dampers.h"
#include "framerate.h"
#include "picture_settings.h"
#include "extradata.h"

#ifdef __DBGLOG
    #include <los/api/los_api.h>
    #define IMG_LOG(a,b,c,d,e,f,g) LOS_Log(a,b,c,d,e,f,g)
#else
    #define IMG_LOG(a,b,c,d,e,f,g)
#endif


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CAM_SM);
#endif

/**
 * Structure to save isp internal config
 * like LR/HR config and isp low level status
 */
typedef struct ISPPrivateData_t {
    t_uint32 Pipe0_XSize;
    t_uint32 Pipe0_YSize;
    t_uint32 Pipe1_XSize;
    t_uint32 Pipe1_YSize;
    t_uint32 Pipe0_OutputFormat;
    t_uint32 Pipe1_OutputFormat;
    t_uint32 Pipe0_Enable;
    t_uint32 Pipe1_Enable;
    t_uint32 ISPLowLevelStatus;
} ISPPrivateData;

class CAM_SM : public COM_SM {

public:

    CAM_SM(ENS_Component &enscomp):
        COM_SM(enscomp),
        bLowPower(FALSE),
        pFlashDriver(NULL),
        ePreset(OMX_SYMBIAN_RawImageProcessed),
        eMirror(OMX_MirrorNone),
        stillSynchro_BufferReceived(false),
        stillSynchro_StatsReceived(false)
{
    currentStillSensorMode.u32_woi_res_width = 0;
    currentStillSensorMode.u32_woi_res_height = 0;
    currentStillSensorMode.u32_output_res_width = 0;
    currentStillSensorMode.u32_output_res_height = 0;
    currentStillSensorMode.u32_data_format = 0;
    currentStillSensorMode.u32_usage_restriction_bitmask = 0;
    currentStillSensorMode.u32_max_frame_rate_x100 = 0;
    pOpModeMgr = NULL;
	StillinVideo =0 ;
	GrabStatusNonDefferred =0 ;
	pipeStartedAfterCapture =0;
	nFramesTobeSkipped =0;
    extradataCircularBuffer_init();
    bMoveToVideo = OMX_FALSE;
    memset(&mIspPrivateData,0,sizeof(ISPPrivateData));
}

protected:

    virtual SCF_STATE ProcessPendingEvents(s_scf_event const *e);

/* ------------------ */
/* Boot state machine */
/* ------------------ */

        virtual SCF_STATE Booting(s_scf_event const *e);
            SCF_STATE InitingXp70WFA(s_scf_event const *e);
            SCF_STATE PreBootingWFA(s_scf_event const *e);
            SCF_STATE PostBootingWFA(s_scf_event const *e);
            SCF_STATE PostBooting_SensorInfo(s_scf_event const *e);
            SCF_STATE SensorModes(s_scf_event const *e);
            SCF_STATE SensorNVM(s_scf_event const *e);
            SCF_STATE FirmwareTuning_CheckIfSupported(s_scf_event const *e);
            SCF_STATE FirmwareTuning_QueryBlockId(s_scf_event const *e);
            SCF_STATE FirmwareTuning_CheckCoin(s_scf_event const *e);
            SCF_STATE FirmwareTuning_ReadBlockId(s_scf_event const *e);
            SCF_STATE FirmwareTuning_LoadData(s_scf_event const *e);
            SCF_STATE FirmwareTuning_SendDataToFirmware(s_scf_event const *e);
            SCF_STATE FlashNVM(s_scf_event const *e);
            SCF_STATE FlashTuning_LoadData(s_scf_event const *e);
            SCF_STATE ImageQualityTuning_LoadData(s_scf_event const *e);
            SCF_STATE Sensor_Patch(s_scf_event const *e);
            SCF_STATE Sensor_SpecificSettings(s_scf_event const *e);
            SCF_STATE Linearization(s_scf_event const *e);
            SCF_STATE Gridiron(s_scf_event const *e);
            SCF_STATE ApplyDefaultOMXConfig(s_scf_event const *e);

			SCF_STATE StopVideoRecord(s_scf_event const *e);

/* --------------- */
/* Camera Features */
/* --------------- */
        SCF_STATE Zooming(s_scf_event const *e);
            SCF_STATE ZoomingExecute(s_scf_event const *e);
            SCF_STATE PanTiltExecute(s_scf_event const *e);
            SCF_STATE ZoomingGetStatus(s_scf_event const *e);
        SCF_STATE ZoomingConfig(s_scf_event const *e);
            SCF_STATE ZoomingConfigExecute(s_scf_event const *e);
        SCF_STATE ZoomBeforeStateExecuting(s_scf_event const *e);
            SCF_STATE ExecuteZoomBeforeStateExecuting(s_scf_event const *e);
            SCF_STATE ExecutePanTiltBeforeStateExecuting(s_scf_event const *e);
            SCF_STATE ZoomingGetStatusBeforeExecuting(s_scf_event const *e);

        SCF_STATE SettingTestMode(s_scf_event const *e);
            SCF_STATE ProcessTestMode(s_scf_event const *e);

        SCF_STATE Rating(s_scf_event const *e);
            SCF_STATE Rating_SetFrameRate(s_scf_event const *e);

        SCF_STATE Mirroring(s_scf_event const *e);
        SCF_STATE MirroringExecute(s_scf_event const *e);

        virtual SCF_STATE PowerUp_STC(s_scf_event const *e);
        SCF_STATE PowerUp_STC_SensorInfo(s_scf_event const *e);
        SCF_STATE PowerUp_STC_GetNbBlockIds(s_scf_event const *e);
        SCF_STATE PowerUp_STC_QueryId(s_scf_event const *e);
        SCF_STATE PowerUp_STC_CheckCoin(s_scf_event const *e); 
        SCF_STATE PowerUp_STC_ReadId(s_scf_event const *e);
        SCF_STATE PowerUp_STC_LoadData(s_scf_event const *e);
        SCF_STATE PowerUp_STC_SendDataToFirmware(s_scf_event const *e);
        SCF_STATE PowerUp_STC_Done(s_scf_event const *e);

        SCF_STATE TimeNudge(s_scf_event const *e);
        SCF_STATE TimeNudge_StartCapture(s_scf_event const *e);
        SCF_STATE TimeNudge_stop3A(s_scf_event const *e);
       SCF_STATE TimeNudge_FlashEnabledStartSw3A(s_scf_event const *e);
       SCF_STATE TimeNudge_StartHiddenCapture(s_scf_event const *e);

/* --------------- */
/* Operating modes */
/* --------------- */
        virtual SCF_STATE EnteringPreview(s_scf_event const *e);
        SCF_STATE EnteringPreview_DoingPreviewConfiguration(s_scf_event const *e);
        SCF_STATE EnteringPreview_ConfiguringZoom(s_scf_event const *e);
        SCF_STATE EnteringPreview_DoingPreviewRate(s_scf_event const *e);
        SCF_STATE EnteringPreview_SendingPreviewIQSettings(s_scf_event const *e);
        SCF_STATE EnteringPreview_Start3A(s_scf_event const *e);
        SCF_STATE EnteringPreview_StartingVpip(s_scf_event const *e);
        SCF_STATE EnteringPreview_ConfiguringZoom2(s_scf_event const *e);
        SCF_STATE EnteringPreview_SendBuffers(s_scf_event const *e);

        SCF_STATE ExitingPreview(s_scf_event const *e);
        SCF_STATE ExitingPreview_StoppingFlash(s_scf_event const *e);
        SCF_STATE ExitingPreview_StoppingVpip(s_scf_event const *e);

        SCF_STATE ExitingRecord(s_scf_event const *e);
        SCF_STATE ExitingRecord_WaitingBuffers(s_scf_event const *e);
        SCF_STATE ExitingRecord_StoppingFlash(s_scf_event const *e);

        SCF_STATE EnteringRawCapture(s_scf_event const *e);
        SCF_STATE EnteringRawCapture_DoingRawCaptureConfiguration(s_scf_event const *e);
        SCF_STATE EnteringRawCapture_ConfiguringZoom(s_scf_event const *e);
        SCF_STATE EnteringRawCapture_DoingRate(s_scf_event const *e);
        SCF_STATE EnteringRawCapture_SendingRawCaptureIQSettings(s_scf_event const *e);
        SCF_STATE EnteringRawCapture_Start3A(s_scf_event const *e);
        SCF_STATE EnteringRawCapture_SendBuffers(s_scf_event const *e);
        SCF_STATE EnteringRawCapture_StartStreaming(s_scf_event const *e);
        SCF_STATE FillBufferDoneVPB1_StoreBMSContext(s_scf_event const *e);
        SCF_STATE FillBufferDoneVPB1_AddExtradata(s_scf_event const *e);

        SCF_STATE ExitingRawCapture(s_scf_event const *e);
        SCF_STATE ExitingRawCapture_StoppingFlash(s_scf_event const *e);
        SCF_STATE ExitingRawCapture_StoppingVpip(s_scf_event const *e);
        SCF_STATE ExitingRawCapture_FreeCamera(s_scf_event const *e);
	    SCF_STATE EnteringPreview_Config_Post_StillinVideo(s_scf_event const *e);


		SCF_STATE EnteringPreview_HiddenBMS_DoingRawCaptureConfiguration(s_scf_event const *e) ;	
		SCF_STATE EnteringPreview_HiddenBMS_Start(s_scf_event const *e);
		SCF_STATE EnteringRawCapture_HiddenBML_Start(s_scf_event const *e) ;
		SCF_STATE EnteringRawCapture_HiddenBMS_DoingRawCaptureBMLConfiguration(s_scf_event const *e) ;

        SCF_STATE  ZSLtoHDR(s_scf_event const *e);
        SCF_STATE  ZSLHDRConfigureCaptureMode(s_scf_event const *e);
        SCF_STATE  ZSLHDRStopPreview(s_scf_event const *e);
        SCF_STATE  ZSLHDRCapture(s_scf_event const *e);
        SCF_STATE  ResumeAfterZSLHDR(s_scf_event const *e);
        SCF_STATE  HDRtoZSL(s_scf_event const *e);

/* ---------------*/
/* Enabling Ports */
/* ---------------*/
        /* TODO : to be ported for V1*/

        virtual SCF_STATE EnablePortInIdleExecutingState(s_scf_event const *e);
            SCF_STATE EnablePortInIdleExecutingState_StopVpip(s_scf_event const *e);
            SCF_STATE EnablePortInIdleExecutingState_ConfigurePipe(s_scf_event const *e);
            SCF_STATE EnablePortInIdleExecutingState_DoingRate(s_scf_event const *e);
            SCF_STATE EnablePortInIdleExecutingState_ConfigureZoom(s_scf_event const *e);
            SCF_STATE EnablePortInIdleExecutingState_StartVpip(s_scf_event const *e) ;
	     SCF_STATE EnablePortInIdleExecutingState_CfgGrab(s_scf_event const *e);
            SCF_STATE EnablePortInIdleExecutingState_SendOmxCb(s_scf_event const *e);

/* ---------------*/
/*For Shutterlag*/
/* ---------------*/

		SCF_STATE RendezvousState(s_scf_event const *e);			


/* -------------*/
/* IQ settings  */
/* -------------*/
        SCF_STATE CAMERA_LoadingIQSettings(s_scf_event const *e);
            SCF_STATE CAMERA_LoadingIQSettings_OpModeSetup(s_scf_event const *e);
            SCF_STATE CAMERA_LoadingIQSettings_ZoomPreRun(s_scf_event const *e);


/* Methods that need to be overriden for configuring pipes */
        virtual OMX_ERRORTYPE configureIspResolution(t_uint8);
        virtual OMX_ERRORTYPE configureResolution(void);
        virtual OMX_ERRORTYPE configurePipes(void);

        SCF_STATE SW3A_Control(s_scf_event const *e);
            SCF_STATE SW3A_StartGrab(s_scf_event const *e);
            SCF_STATE SW3A_ControlComputed(s_scf_event const *e);
            SCF_STATE SW3A_ControlGammaLUT(s_scf_event const *e);
                SCF_STATE SW3A_ControlGammaLUT_WakeUp(s_scf_event const *e);
                SCF_STATE SW3A_ControlGammaLUT_Stop(s_scf_event const *e);
                SCF_STATE SW3A_ControlGammaLUT_Start(s_scf_event const *e);
                SCF_STATE SW3A_ControlGammaLUT_Write(s_scf_event const *e);

        SCF_STATE SW3A_Start(s_scf_event const *e);
            SCF_STATE SW3A_Starting(s_scf_event const *e);
            SCF_STATE SW3A_RunPreCaptureFlashes(s_scf_event const *e);


        SCF_STATE SW3A_Stop(s_scf_event const *e);
            SCF_STATE SW3A_Stopping(s_scf_event const *e);


        SCF_STATE SW3A_GetFwStatus(s_scf_event const *e);
	SCF_STATE SW3A_SetFwStatusOn(s_scf_event const *e);
	SCF_STATE SW3A_SetFwStatusOff(s_scf_event const *e);
        SCF_STATE ExecutingToIdle(s_scf_event const *e);
            SCF_STATE ExecutingToIdle_3(s_scf_event const *e);

    virtual bool timeoutAction(void);
    OMX_ERRORTYPE setVPB1Extradata(OMX_BUFFERHEADERTYPE* pOmxBufHdr, bool inBuffer);

    static void flashGetNVMCallBack(TFlashReturnCode aReturnStatus, void* pThis);

public :
    void startLowPowerTimer();
    void stopLowPowerTimer();
    virtual void SW3A_start(SCF_PSTATE whatNext);
    virtual void SW3A_stop(SCF_PSTATE whatNext);
    const char *get_state_str(const SCF_PSTATE state);

    /* Feature related */
    CZoom * pZoom;
    CTestMode * pTestMode;

    /* Tuning */
    CTuningLoaderManager* pTuningLoaderManager;
    CTuningDataBase* pTuningDataBase;
    CIspDampers* pIspDampers;
    CPictureSettings* pPictureSettings;

    // Sensor related
    CSensor* pSensor;
    t_uint16 iNvmSize;
    CFramerate* pFramerate;

    /*For Shutterlag*/		
    t_sw3a_stop_req_status Sw3aStopReqStatus;
    t_grab_abort_status GrabAbortStatus;

    // LowPower
    bool bLowPower;

    // Shared memory related
    CCamSharedMemory* pSharedMemory;
    camSharedMemChunk_t* pSensorModesShmChunk;
    camSharedMemChunk_t* pNvmShmChunk;
    camSharedMemChunk_t* pFwShmChunk;

    camSharedMemChunk_t* pGammaSharpGreen_LR;
    camSharedMemChunk_t* pGammaSharpRed_LR;
    camSharedMemChunk_t* pGammaSharpBlue_LR;
    camSharedMemChunk_t* pGammaUnSharpGreen_LR;
    camSharedMemChunk_t* pGammaUnSharpRed_LR;
    camSharedMemChunk_t* pGammaUnSharpBlue_LR;

    camSharedMemChunk_t* pGammaSharpGreen_HR;
    camSharedMemChunk_t* pGammaSharpRed_HR;
    camSharedMemChunk_t* pGammaSharpBlue_HR;
    camSharedMemChunk_t* pGammaUnSharpGreen_HR;
    camSharedMemChunk_t* pGammaUnSharpRed_HR;
    camSharedMemChunk_t* pGammaUnSharpBlue_HR;

    CResourceSharerManager* pResourceSharerManager;

    /* Operating mode related */
    t_uint16 RecordPort;
    t_uint16 CapturePort;
    t_uint16 RawCapturePort;
    CopModeMgr * pOpModeMgr;
    CGrabControl * pGrabControl;
    CGrab * pGrab;
    CIspctlComponentManager * pIspctlComponentManager;
    CCapture_context * pCapture_context;
    Sw3AProcessingComp * p3AComponentManager;
	Flag_te Configured_RxAbort_OnStop;
    CFlashDriver * pFlashDriver;
    OMX_U32 StillinVideo ;
	OMX_U32 GrabStatusNonDefferred  ;
  	OMX_U32 pipeStartedAfterCapture ;
	OMX_U32 nFramesTobeSkipped ;
	struct timeval tempo1;

    t_uint8 enabledPort;
    CSelfTest * pSelfTest;

			

    OMX_SYMBIAN_RAWIMAGEPRESETTYPE ePreset;
    OMX_MIRRORTYPE eMirror;
    //OMX_S32 nRotation;

    //OMX_SYMBIAN_CAPTUREPARAMETERSTYPE extradataCaptureParameters;
    
    Sensor_Output_Mode_ts currentStillSensorMode;

    /* circular buffer of EXTRADATA_STILLPACKSIZE extradatas (time nudge only) */
    void extradataCircularBuffer_init();
    void extradataCircularBuffer_startEmptying();
    OMX_BOOL extradataCircularBuffer_isEmptying();
    OMX_U8* extradataCircularBuffer_pop();

    ISPPrivateData mIspPrivateData;
protected:
    SCF_STATE EnteringSelfTest(s_scf_event const *e);
    SCF_STATE ProcessingSelfTest(s_scf_event const *e);

    OMX_BOOL  CheckRendezvousCondition();


private :
    t_bool stillSynchro_BufferReceived;
    t_bool stillSynchro_StatsReceived;

    /* circular buffer of EXTRADATA_STILLPACKSIZE extradatas (time nudge only) */
    OMX_U8 ExtradataBuffer[MAX_ALLOCATED_BUFFER_NB][EXTRADATA_STILLPACKSIZE];
    OMX_U8 ExtradataBufferPtr;
    OMX_BOOL EmptyExtradataBuffer;
    OMX_U8 ExtradataBufferElements;
    OMX_U8 ExtradataBufferSize;
    OMX_BOOL bMoveToVideo;

    // Fw tuning related
    t_uint16  iFwSubBlockIdsCount;
    t_uint16* pFwSubBlockId;
    t_uint16  iCurrentFwSubBlockIndex;
    Coin_te   iStatusCoin;
    int       iStatusCoinRetriesCount;

    s_scf_event lastUserDefferedEventDequeued;
};

#endif

