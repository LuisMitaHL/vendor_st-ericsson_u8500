/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//#undef OMXCOMPONENT
//#define OMXCOMPONENT "OPMODEMGR"
#define DBGT_PREFIX "OPMODEMGR"



#include "opmodemgr.h"
#include "omxcamera.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_opmode_transition_opmodemgrTraces.h"
#endif

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


/* constructor */
CopModeMgr::CopModeMgr(ENS_Component* pENSComp):
    UserBurstNbFrames(0)
    ,CameraBurstNbFrames(0)
    ,NumberOfBuffersForStill(0)
    ,NeedToStop3A(OMX_FALSE)
    ,ConfigCapturePort(1)
    ,AutoPauseAfterCapture(OMX_FALSE)
    ,nFrameBefore(1)
    ,bPrepareCapture(OMX_FALSE)
{
    u32_frameLimit = 1;;
    u32_bracketLimit = 0;
    bBurstFrameLimited = true;

    CurrentOperatingMode = OpMode_Cam_StillPreview;
    captureRequest[CAMERA_PORT_OUT0] = 0;
    captureRequest[CAMERA_PORT_OUT1] = 0;
    captureRequest[CAMERA_PORT_OUT2] = 0;
    ConfigCapturing[CAMERA_PORT_OUT0] =OMX_FALSE;
    ConfigCapturing[CAMERA_PORT_OUT1] =OMX_FALSE;
    ConfigCapturing[CAMERA_PORT_OUT2] =OMX_FALSE;

    isEOS[CAMERA_PORT_OUT0]=OMX_FALSE;
    isEOS[CAMERA_PORT_OUT1]=OMX_FALSE;
    isEOS[CAMERA_PORT_OUT2]=OMX_FALSE;


    nWaitingForBuffer[CAMERA_PORT_OUT0]=OMX_FALSE;
    nWaitingForBuffer[CAMERA_PORT_OUT1]=OMX_FALSE;
    nWaitingForBuffer[CAMERA_PORT_OUT2]=OMX_FALSE;

    nBufferAtMPC[CAMERA_PORT_OUT0]=0;
    nBufferAtMPC[CAMERA_PORT_OUT1]=0;
    nBufferAtMPC[CAMERA_PORT_OUT2]=0;

    pTuningDataBase = NULL;
    mViewfinderPort = (camport*)(NULL);
    mRawCapturePort = (camport*)(NULL);
    mRecordPort = (camport*)(NULL);
    PreviousOperatingMode = OpMode_Cam_Unknown;
    mStabEnabled = OMX_FALSE;

    pENSComponent = pENSComp;
}

void CopModeMgr::SetTuningDataBase(CTuningDataBase* aTuningDataBase) {
	pTuningDataBase = aTuningDataBase;
}

void CopModeMgr::UpdateCurrentOpMode() {
	IN0("\n");
	OstTraceFiltStatic0(TRACE_FLOW, "Entry CopModeMgr::UpdateCurrentOpMode", mViewfinderPort);
	switch(CurrentOperatingMode) {
		case OpMode_Cam_StillPreview:
			PreviousOperatingMode = OpMode_Cam_StillPreview;

			if(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_TRUE)
			{
				CurrentOperatingMode = OpMode_Cam_StillFaceTracking;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) && IsLimited() && (1==GetFrameLimit())) // Single Capture
			{
				CurrentOperatingMode = OpMode_Cam_StillCaptureSingle;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureSingle - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureSingle - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) && IsLimited()) // Limited Burst Capture
			{
                CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimited;
                MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d\n", CurrentOperatingMode);
                OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d", mViewfinderPort, CurrentOperatingMode);
			}
            else if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) && !IsLimited()) // Infinite Burst Capture
            {
                CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfinite;
                MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d\n", CurrentOperatingMode);            
                OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d", mViewfinderPort, CurrentOperatingMode);
            }
			else {
                /* OperatingMode has not changed */
				}
			break;

		case OpMode_Cam_VideoPreview:
			PreviousOperatingMode = OpMode_Cam_VideoPreview;

			if(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_TRUE) {
				CurrentOperatingMode = OpMode_Cam_VideoRecord;
				MSG1("CurrentOperatingMode is OpMode_Cam_VideoRecord - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoRecord - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) && IsLimited() && (1==GetFrameLimit())) {
				CurrentOperatingMode = OpMode_Cam_StillCaptureSingleInVideo;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureSingleInVideo - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureSingleInVideo - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) && IsLimited()) // Limited Burst Capture
			{
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimitedInVideo;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d", mViewfinderPort, CurrentOperatingMode);
			}
            else if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) && !IsLimited()) // Infinite Burst Capture
            {
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfiniteInVideo;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else {
                /* OperatingMode has not changed */
            }
			break;
		case OpMode_Cam_VideoRecord:
			PreviousOperatingMode = OpMode_Cam_VideoRecord;
			if(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE) {
				CurrentOperatingMode = OpMode_Cam_VideoPreview;
				MSG1("CurrentOperatingMode is OpMode_Cam_VideoPreview - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoPreview - %d", mViewfinderPort, CurrentOperatingMode);
			}

			else if(ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) {
				if(IsLimited() && (1==GetFrameLimit())) {
					CurrentOperatingMode = OpMode_Cam_StillCaptureSingleInVideo;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureSingleInVideo - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureSingleInVideo - %d", mViewfinderPort, CurrentOperatingMode);
				}
				else if(IsLimited()) {
					CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimitedInVideo;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d", mViewfinderPort, CurrentOperatingMode);
				}
				else if(!IsLimited()) {
					CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfiniteInVideo;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d", mViewfinderPort, CurrentOperatingMode);
				}
                else {
                	DBC_ASSERT(0);
                }
			}
			else {
                /* OperatingMode has not changed */
            }
			break;
		case OpMode_Cam_StillFaceTracking:
			PreviousOperatingMode = OpMode_Cam_StillFaceTracking;
			if(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE) {
				CurrentOperatingMode = OpMode_Cam_StillPreview;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillPreview - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillPreview - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if(ConfigCapturing[CAMERA_PORT_OUT1]==OMX_TRUE) {
				if(IsLimited() && (1==GetFrameLimit())) {
					CurrentOperatingMode = OpMode_Cam_StillCaptureSingle;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureSingle - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureSingle - %d", mViewfinderPort, CurrentOperatingMode);
				}
				else if(IsLimited()) {
					CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimited;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d", mViewfinderPort, CurrentOperatingMode);
				}
				else if(!IsLimited()) {
					CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfinite;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d", mViewfinderPort, CurrentOperatingMode);
				}
                else {
                	DBC_ASSERT(0);
                }
			}
			else {
                /* OperatingMode has not changed */
            }
			break;
		case OpMode_Cam_StillCaptureSingle:
			if(ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE) {
				if(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE){
					CurrentOperatingMode = OpMode_Cam_StillPreview;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillPreview - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillPreview - %d", mViewfinderPort, CurrentOperatingMode);
				}
				else
				{
					CurrentOperatingMode = OpMode_Cam_StillFaceTracking;
					MSG1("CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d", mViewfinderPort, CurrentOperatingMode);
				}
			}
			else {
                /* OperatingMode has not changed */
            }
			break;
		case OpMode_Cam_StillCaptureSingleInVideo:
			if(ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE) {
				if(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE)
				{
					CurrentOperatingMode = OpMode_Cam_VideoPreview;
					MSG1("CurrentOperatingMode is OpMode_Cam_VideoPreview - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoPreview - %d", mViewfinderPort, CurrentOperatingMode);
				}
				else
				{
					CurrentOperatingMode = OpMode_Cam_VideoRecord;
					MSG1("CurrentOperatingMode is OpMode_Cam_VideoRecord - %d\n", CurrentOperatingMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoRecord - %d", mViewfinderPort, CurrentOperatingMode);
				}
			}
			else {
                /* OperatingMode has not changed */
			}
			break;
		case OpMode_Cam_StillCaptureBurstInfinite:
			if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE)) {
				CurrentOperatingMode = OpMode_Cam_StillPreview;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillPreview - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillPreview - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_TRUE)) {
				CurrentOperatingMode = OpMode_Cam_StillFaceTracking;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if (!IsLimited()) {
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfinite;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d\n", CurrentOperatingMode);
			}
			else
			{
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimited;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d", mViewfinderPort, CurrentOperatingMode);
			}
			break;
		case OpMode_Cam_StillCaptureBurstLimited:

			if((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE)) {
				CurrentOperatingMode = OpMode_Cam_StillPreview;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillPreview - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillPreview - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_TRUE)) {
				CurrentOperatingMode = OpMode_Cam_StillFaceTracking;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillFaceTracking - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if (IsLimited()) {
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimited;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d\n", CurrentOperatingMode);
			}
			else{
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfinite;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d", mViewfinderPort, CurrentOperatingMode);
			}
			break;
		case OpMode_Cam_StillCaptureBurstInfiniteInVideo:
			if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE)) {
				CurrentOperatingMode = OpMode_Cam_VideoPreview;
				MSG1("CurrentOperatingMode is OpMode_Cam_VideoPreview - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoPreview - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_TRUE)) {
				CurrentOperatingMode = OpMode_Cam_VideoRecord;
				MSG1("CurrentOperatingMode is OpMode_Cam_VideoRecord - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoRecord - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if (!IsLimited()) {
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfiniteInVideo;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d\n", CurrentOperatingMode);
			}
			else
			{
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimitedInVideo;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d", mViewfinderPort, CurrentOperatingMode);
			}
			break;
		case OpMode_Cam_StillCaptureBurstLimitedInVideo:
			if((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_FALSE)) {
				CurrentOperatingMode = OpMode_Cam_VideoPreview;
				MSG1("CurrentOperatingMode is OpMode_Cam_VideoPreview - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoPreview - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if ((ConfigCapturing[CAMERA_PORT_OUT1]==OMX_FALSE)&&(ConfigCapturing[CAMERA_PORT_OUT2]==OMX_TRUE)) {
				CurrentOperatingMode = OpMode_Cam_VideoRecord;
				MSG1("CurrentOperatingMode is OpMode_Cam_VideoRecord - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_VideoRecord - %d", mViewfinderPort, CurrentOperatingMode);
			}
			else if (IsLimited()) {
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimitedInVideo;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d\n", CurrentOperatingMode);
			}
			else{
				CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfiniteInVideo;
				MSG1("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d\n", CurrentOperatingMode);
				OstTraceFiltStatic1(TRACE_DEBUG, "CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d", mViewfinderPort, CurrentOperatingMode);
			}
			break;

		default :
			DBC_ASSERT(0); //operating mode has been corrupted.
	}
	OUTR(" ",0);
	OstTraceFiltStatic0(TRACE_FLOW, "Exit CopModeMgr::UpdateCurrentOpMode", mViewfinderPort);
}

void CopModeMgr::getStabPorts(camport **portVF, camport **portRecord, OMX_U32 *HRColorFormat)
{
    if (CurrentOperatingMode == OpMode_Cam_VideoPreview) {
        (*portVF) = (*portRecord) = mViewfinderPort;
    } else {
        (*portVF) = mViewfinderPort;
        (*portRecord) = mRecordPort; 
    }
    (*HRColorFormat) = mRecordPort->getParamPortDefinition().format.video.eColorFormat;
} 

void CopModeMgr::getPorts(camport **portVF, camport **portRecord)
{
    (*portVF) = mViewfinderPort;
    (*portRecord) = mRecordPort; 
} 

/** 
* \brief compute the number of consecutive buffers we will grab before doing BML
* \in 
* \out
* \return OMX_ErrorNone if ok
*/
OMX_ERRORTYPE CopModeMgr::Compute_NumberOfBuffersForStill(void)
{
	/* no buffer available */
	if (0 == nBufferAtMPC[CAMERA_PORT_OUT1])
    {
    	NumberOfBuffersForStill = 1;
    }
    else
    {
    	/* single still */
    	if ((CurrentOperatingMode == OpMode_Cam_StillCaptureSingle)
					||(CurrentOperatingMode == OpMode_Cam_StillCaptureSingleInVideo))
        {
        	NumberOfBuffersForStill = 1;
        }
        /* infinite burst : grab as many buffers as possible */
        else if ((CurrentOperatingMode == OpMode_Cam_StillCaptureBurstInfinite)
					||(CurrentOperatingMode == OpMode_Cam_StillCaptureBurstInfiniteInVideo))
        {
        	NumberOfBuffersForStill = nBufferAtMPC[CAMERA_PORT_OUT1];
        }
        /* limited burst */
        else if ((CurrentOperatingMode == OpMode_Cam_StillCaptureBurstLimited)
					||(CurrentOperatingMode == OpMode_Cam_StillCaptureBurstLimitedInVideo))
        {
        	if (nBufferAtMPC[CAMERA_PORT_OUT1] < (GetFrameLimit() - UserBurstNbFrames))
            {
            	NumberOfBuffersForStill = nBufferAtMPC[CAMERA_PORT_OUT1];
            }
            else
            {
            	NumberOfBuffersForStill = GetFrameLimit() - UserBurstNbFrames;
            }
        }
        /* time nudge (when/after capturing bit is set only) */
        else if (OMX_TRUE == IsTimeNudgeEnabled())
        {
            if (nBufferAtMPC[CAMERA_PORT_OUT1] < (GetFrameLimit() + nFrameBefore - UserBurstNbFrames))
            {
                NumberOfBuffersForStill = nBufferAtMPC[CAMERA_PORT_OUT1];
            }
            else
            {
                NumberOfBuffersForStill = GetFrameLimit() + nFrameBefore - UserBurstNbFrames;
            }
        }
        else
        {
        	NumberOfBuffersForStill = 0;
            return (OMX_ErrorUndefined);
        }
    }
    
    return (OMX_ErrorNone);
}


CTuning* CopModeMgr::GetOpModeTuning()
{
	CTuning* pTuning = NULL;
	t_tuning_object iTuningObj = TUNING_OBJ_STILL_PREVIEW;
	switch(CurrentOperatingMode)
	{
		case OpMode_Cam_Unknown:
			DBC_ASSERT(0);
			break;
		case OpMode_Cam_StillPreview:
		case OpMode_Cam_StillFaceTracking:
			if (OMX_FALSE == bPrepareCapture)
			{
				iTuningObj = TUNING_OBJ_STILL_PREVIEW;
			}
			else
			{
				iTuningObj = TUNING_OBJ_STILL_CAPTURE;
			}
			break;
		case OpMode_Cam_VideoPreview:
			iTuningObj = TUNING_OBJ_VIDEO_PREVIEW;
			break;
		case OpMode_Cam_VideoRecord:
			iTuningObj = TUNING_OBJ_VIDEO_RECORD;
			break;
		case OpMode_Cam_StillCaptureSingle:
		case OpMode_Cam_StillCaptureSingleInVideo:
		case OpMode_Cam_StillCaptureBurstInfinite:
		case OpMode_Cam_StillCaptureBurstLimited:
		case OpMode_Cam_StillCaptureBurstInfiniteInVideo:
		case OpMode_Cam_StillCaptureBurstLimitedInVideo:
			iTuningObj = TUNING_OBJ_STILL_CAPTURE;
			break;
		default :
			DBC_ASSERT(0);
			break;
	}
	pTuning = pTuningDataBase->getObject(iTuningObj);
	MSG1("OpMode tuning instance: %s\n", pTuning!=NULL?pTuning->GetInstanceName():"unknown");
	//OstTraceFiltStatic1(TRACE_DEBUG, "OpMode tuning instance: %s", mViewfinderPort, pTuning!=NULL?pTuning->GetInstanceName():"unknown");
	return pTuning;
}

t_uint32 CopModeMgr::GetFrameLimit()
{   // If bracketing is configured, the limited burst length has to reflect it.
    // The CaptureMode burst configuration will then be ignored until bracketing is reset.
    return (u32_bracketLimit > 0) ? u32_bracketLimit : u32_frameLimit;
}


bool CopModeMgr::IsLimited()
{
    return (u32_bracketLimit > 0) ? true : bBurstFrameLimited;
}

OMX_BOOL CopModeMgr::IsTimeNudgeEnabled(void)
{
    if ((OMX_TRUE == bPrepareCapture)
        && (OMX_TRUE == ((COmxCamera*)pENSComponent)->mSensorMode.bOneShot))
    {
        return OMX_TRUE;
    }
    else
    {
        return OMX_FALSE;
    }
}

t_uint32 CopModeMgr::GetBurstNberOfFrames(void)
{
    if ((OMX_TRUE == bPrepareCapture)
        && ((OpMode_Cam_StillPreview == CurrentOperatingMode)
            || (OpMode_Cam_StillFaceTracking == CurrentOperatingMode)))
    {
        return (u32_frameLimit + nFrameBefore);
    }
    else
    {
        return u32_frameLimit;
    }
}

#define MAKESTRING(mode) case mode: strMode = #mode; break
const char* CopModeMgr::getOpModeStr()
{
    IN0();
    const char *strMode;
    const t_operating_mode_camera operatingMode = CurrentOperatingMode;

    switch(operatingMode) {
        MAKESTRING(OpMode_Cam_Unknown);
        MAKESTRING(OpMode_Cam_VideoPreview);
        MAKESTRING(OpMode_Cam_VideoRecord);
        MAKESTRING(OpMode_Cam_StillPreview);
        MAKESTRING(OpMode_Cam_StillCaptureSingle);
        MAKESTRING(OpMode_Cam_StillCaptureBurstLimited);
        MAKESTRING(OpMode_Cam_StillCaptureBurstInfinite);
        MAKESTRING(OpMode_Cam_StillFaceTracking);
        MAKESTRING(OpMode_Cam_StillCaptureSingleInVideo);
        MAKESTRING(OpMode_Cam_StillCaptureBurstLimitedInVideo);
        MAKESTRING(OpMode_Cam_StillCaptureBurstInfiniteInVideo);
        default:
            strMode = "Invalid Operating Mode";
            break;
    }

    OUT0();
    return strMode;
}
