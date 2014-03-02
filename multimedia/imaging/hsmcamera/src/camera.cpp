/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* +CR337836 */
#define DECLARE_AUTOVAR
/* -CR337836 */
#ifndef CR_ZSL_YUV
#define CR_ZSL_YUV
#endif
#define DBGT_PREFIX "CAMERA"

#include "camera.h"
#include "omxcamera.h"
#include "ENS_Nmf.h"
#include "ENS_Component_Fsm.h"
#include "OMX_CoreExt.h"
#include "OMX_IndexExt.h"
#include "string.h"
#include "SharedBuffer.h"
#include "isp_firmware_primary.h"
#include "isp_firmware_secondary.h"
#include "cam_shared_memory.h"
#include "extradata.h"
#include "omxilosalservices.h"
#include <time.h>

#include <sys/time.h>

#ifndef SUBTIMINGS
#define SUBTIMINGS
#endif


#ifdef FACE_DETECTOR_TEST_DRAWOBJECTS_ON
#include "imgTest_draw.h"
#endif /* FACE_DETECTOR_TEST_DRAWOBJECTS_ON */


#include "osi_trace.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_cameraTraces.h"
#endif

extern "C"{
#include <cm/inc/cm_macros.h>
#include <cm/inc/cm.hpp>
#ifdef __DBGLOG
    #include <los/api/los_api.h>
    #define IMG_LOG(a,b,c,d,e,f,g) LOS_Log(a,b,c,d,e,f,g)
#else
    #define IMG_LOG(a,b,c,d,e,f,g)
#endif
}

#define CAMERA_TUNING_DATABASE_CLIENT_NAME "CAMERA"

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8
// Check alive tracing defaults
#ifdef __DEBUG
#define DEFAULT_CHECK_ALIVE_INTERVAL_MSEC 15000 // zero is disabled
#define DEFAULT_CHECK_ALIVE_PRINT_STATUS OMX_TRUE
#else
#define DEFAULT_CHECK_ALIVE_PRINT_STATUS OMX_FALSE
#ifdef ENABLE_LOWPOWER_ISP
#define DEFAULT_CHECK_ALIVE_INTERVAL_MSEC 500 // zero is disabled
#else
#define DEFAULT_CHECK_ALIVE_INTERVAL_MSEC 0 // zero is disabled
#endif
#endif /* __DEBUG */

#define OMX_VERSION_1_1_2 0x00020101

// Local prototypes
static void Camera_ispctlInfo(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd);
const char* stringifyISPState(HostInterfaceLowLevelState_te aHostInterfaceLowLevelState);
const char* stringifyGrabPipeConfig(e_grabPipeID aGrabPipeID);
const char* stringifyISPOutputFormat(OutputFormat_te outputFormat);
//#define TRACE_VPB

void Camera::fillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer) {

	switch (pBuffer->nOutputPortIndex) {
			case (CAMERA_PORT_OUT0) :
			{
			    IMG_TIME_LOG(IMG_TIME_VPB0);
				iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT0]--;
				MSG2("FBDVPB0 iOpModeMgr.nBufferAtMPC[%ld] = %d\n",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
				OstTraceFiltInst2(TRACE_DEBUG, "FBDVPB0 iOpModeMgr.nBufferAtMPC[%ld] = %d",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
				fillBufferDoneVPB0(pBuffer);
				break;
			}
			case (CAMERA_PORT_OUT1) :
			{
				iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT1]--;
				MSG2("FBDVPB1 iOpModeMgr.nBufferAtMPC[%ld] = %d\n",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
				OstTraceFiltInst2(TRACE_DEBUG, "FBDVPB1 iOpModeMgr.nBufferAtMPC[%ld] = %d",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
				fillBufferDoneVPB1(pBuffer);
				break;
			}
			case (CAMERA_PORT_OUT2) :
			{
				iOpModeMgr.nBufferAtMPC[CAMERA_PORT_OUT2]--;
				MSG2("FBDVPB2 iOpModeMgr.nBufferAtMPC[%ld] = %d\n",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
				OstTraceFiltInst2(TRACE_DEBUG, "FBDVPB2 iOpModeMgr.nBufferAtMPC[%ld] = %d",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
				fillBufferDoneVPB2(pBuffer);
				break;
			}
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
      case (CAMERA_PORT_STATS_AEWB) :
      {
        MSG1("AEWB stats callback: pBuffer = %p\n", pBuffer);
        fillBufferDoneStatsAewb(pBuffer);
        break;
      }
      case (CAMERA_PORT_STATS_AF) :
      {
        MSG1("AF stats callback: pBuffer = %p\n", pBuffer);
        fillBufferDoneStatsAf(pBuffer);
        break;
      }
#endif // CAMERA_ENABLE_OMX_STATS_PORT
			default :
				DBC_ASSERT(0);

    }
}

/* */
/* Camera::fillBufferDoneVPB1 : Still/Burst Capture call-back
 * TODO: Update behaviour for StillDuringVideoRecord
 */
/**/
void Camera::fillBufferDoneVPB1(OMX_BUFFERHEADERTYPE* pOmxBuf) {

    ASYNC_IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::fillBufferDoneVPB1");
    camport* pPort = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT1);

#ifdef CAM_STILL_SYNCHRO_DBG
    printf("CAM_STILL_SYNCHRO_DBG: Camera::fillBufferDoneVPB1\n");
#endif /* CAM_STILL_SYNCHRO_DBG */

    if(0 != pOmxBuf->nFilledLen) {

    	//performance traces start
    	p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
    	mlatency_Hsmcamera.e_OMXCAM_VPB1FrameRate.t0 = p_cam_sm->mTime;
    	mlatency_Hsmcamera.e_OMXCAM_VPB1FrameRate.t1 = p_cam_sm->mTime;
    	mlatency_Hsmcamera.e_OMXCAM_ShotToCapture.t0 = p_cam_sm->mTime;
		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_VPB1FrameRate %d", 0);
		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_VPB1FrameRate %d", 1);
		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToCapture %d", 0);
    	//performance traces end
    	
        OMX_U64 frameId = pOmxBuf->nTimeStamp;
        frameId = (OMX_U8)(frameId >> 56);
        //MSG2("Camera::fillBufferDoneVPB1 frameId : 0x%llx, flags : 0x%04lx\n", frameId, pOmxBuf->nFlags); // Error with OST Dictionary
        //OstTraceFiltInst2(TRACE_DEBUG, "Camera::fillBufferDoneVPB1 frameId : 0x%llx, flags : 0x%04lx", frameId, pOmxBuf->nFlags);
        //MSG2("Camera::fillBufferDoneVPB1 frameId : 0x%x, flags : 0x%x\n", (OMX_U8)frameId, pOmxBuf->nFlags);
        OstTraceFiltInst2(TRACE_DEBUG, "Camera::fillBufferDoneVPB1 frameId : 0x%x, flags : 0x%x", (OMX_U8)frameId, pOmxBuf->nFlags);
        /* store OMX buffer address */
        OMX_ERRORTYPE error = iCapture_context.pushOmxBufHdr(pOmxBuf);
        if (OMX_ErrorNone != error)
        {
            DBC_ASSERT(0);
        }

        if (OMX_TRUE == iOpModeMgr.IsTimeNudgeEnabled()) {
            p_cam_sm->extradataCircularBuffer_startEmptying();
        }

		if(p_cam_sm->StillinVideo!=1)
		{		
        s_scf_event event;
        event.type.other=0;
        event.sig = CAMERA_FILLBUFFERDONE_SIG;
        iDeferredEventMgr.queuePriorEvent(&event);

        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_cam_sm->ProcessEvent(&devent);

        pPort->incrementNumberOfFramesGrabbed();
        pPort->incrementNumberOfFramesStreamedOut();
		}
		else
		{
			p_cam_sm->StillinVideo=0;
			PERF_TRACE_LAT("Buffer on VPB1",p_cam_sm->tempo1);
			s_scf_event event;
	        event.type.other=0;
	        event.sig = ISPPROC_EXIT_CAPTURE_SIG;
	        iDeferredEventMgr.queuePriorEvent(&event);

	        s_scf_event devent;
	        devent.sig = Q_DEFERRED_EVENT_SIG;
	        p_cam_sm->ProcessEvent(&devent);
		}

    } else {
        mENSComponent.fillBufferDone(pOmxBuf);
    }

    ASYNC_OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::fillBufferDoneVPB1");
}

/* */
/* Camera::fillBufferDoneVPB2 : Call-back for Recorded buffer
 */
/**/
void Camera::fillBufferDoneVPB2(OMX_BUFFERHEADERTYPE* pOmxBufHdr)
{
    ASYNC_IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::fillBufferDoneVPB2");
    camport* pPort = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT2);
	PERF_TRACE_LAT("Buffer on VPB2",p_cam_sm->tempo1);
	//EOS not required in still during video record as it would lead to faulty transitions.
	if(p_cam_sm->StillinVideo==1)
		pOmxBufHdr->nFlags &= ~OMX_BUFFERFLAG_EOS;
     MSG1("%s: Filled Length: %lu",__FUNCTION__,pOmxBufHdr->nFilledLen);
    if((pOmxBufHdr->nFlags & OMX_BUFFERFLAG_EOS) != 0) {
     /* The EOS buffer must always be streamed out to the port for datapath consistency reasons.
         * However, there is no guarantee that its contents is applicable to the downstream component.
         * It may be the case for instance if AEC has not converged yet, but the streaming is requested to end.
         * In this case, better clear the buffer contents.
         */
        pOmxBufHdr->nFilledLen = 0;
        pOmxBufHdr->nFlags &= ~OMX_BUFFERFLAG_EXTRADATA;

        s_scf_event event;
        event.type.other=CAMERA_PORT_OUT2;
        event.sig = CAMERA_EOS_VPB02_SIG;
        /* we're misusing the capturingStruct to say where this event came from. In SM, it's easier
        to do one event with param than two events*/
        event.type.capturingStruct.nPortIndex=CAMERA_PORT_OUT2;
        iDeferredEventMgr.queuePriorEvent(&event);

        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_cam_sm->ProcessEvent(&devent);

        if (iOpModeMgr.ConfigCapturing[RecordPort]) {
            /* this port is still capturing, so this means that EOS was generated
            internally as part of VideoRecord->StillRawCapture transition. Hide it from user */
            pOmxBufHdr->nFlags = 0;
        } else {
            /* TODO: could this be done in waitEvent EOS_VPB02_SIG instead? That way we minimize
            operations on callback, and maximize in state machine. */
            iOpModeMgr.captureRequest[RecordPort] --;
        }
    }

	if(0 != pOmxBufHdr->nFilledLen)
	{
		//performance traces start
		p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
		mlatency_Hsmcamera.e_OMXCAM_VPB2FrameRate.t0 = p_cam_sm->mTime;
		mlatency_Hsmcamera.e_OMXCAM_VPB2FrameRate.t1 = p_cam_sm->mTime;
		//performance traces end
    }

    // Recycle the buffer if it should not be passed to the next OMX component.
    OMX_STATETYPE state = pPort->getPortFsm()->getState();

    if((state == OMX_StateExecuting) && pPort->isEnabled()==OMX_TRUE)
    {
        /* In Executing skip needed frames, according to framerate constraints and AEC convergence.
         * NB: EOS buffer shall not be recycled.
         */
        pPort->incrementNumberOfFramesGrabbed();
        if ((((pPort->getNumberOfFramesGrabbed() % iFramerate.getFrameSkipRatio(2)) == 0)
               && ((i3AComponentManager.getAecStateConverged() == SW3A_AEC_CONVERGENCE_STATUS_CONVERGED)
                      || (i3AComponentManager.getAecStateConverged() == SW3A_AEC_CONVERGENCE_STATUS_TIMEOUT)
                      || (OMX_STE_TestModeNone != iTestMode.eCamTestMode)))
                || ((pOmxBufHdr->nFlags & OMX_BUFFERFLAG_EOS) != 0)) {

            COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;

            /* New frame to be pushed out of port */
            pPort->incrementNumberOfFramesStreamedOut();

            /* Add extradata */
            OMX_ERRORTYPE error = OMX_ErrorNone;
            OMX_VERSIONTYPE nVersion;
            nVersion.nVersion = OMX_VERSION;

            /* Camera User Settings */
            error = Extradata::SetExtradataField(pOmxBufHdr, &pOmxCamera->mCameraUserSettings, nVersion);
            if (OMX_ErrorNone != error)
            {
                DBC_ASSERT(0);
            }

            /* Capture Parameters extradata */
            OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pCaptureParameters = i3AComponentManager.getCaptureParameters(false);
            if (pCaptureParameters != NULL) {
                error = Extradata::SetExtradataField(pOmxBufHdr, pCaptureParameters, nVersion);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }
            
            OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraCaptureParameters = i3AComponentManager.getSteExtraCaptureParameters(false);
            if (pSteExtraCaptureParameters != NULL) {
                error = Extradata::SetExtradataField(pOmxBufHdr, pSteExtraCaptureParameters, nVersion);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }

        // Pack the capture parameters for EXIF
        IFM_EXIF_SETTINGS sExifSettings;

        omxilosalservices::OmxIlOsalDateTime::OmxIlOsalDateTimeType sOmxOsalDateTime;

	//Fixing Coverity issue of uninitialised sOmxOsalDateTime
	sOmxOsalDateTime.year = 0;
	sOmxOsalDateTime.month = 0;
	sOmxOsalDateTime.day = 0;
	sOmxOsalDateTime.minutes = 0;
	sOmxOsalDateTime.seconds = 0;
       IFM_DATE_TIME sCurrentDateTime;
       if (!(pOmxCamera->mExifTimeStamp.nTimestamp == 0))
       {
            time_t timeLowPart = pOmxCamera->mExifTimeStamp.nTimestamp;
            struct tm *tmDateTime;

            tmDateTime = localtime(&timeLowPart);

            sCurrentDateTime.year = tmDateTime->tm_year + 1900;
            sCurrentDateTime.month    = tmDateTime->tm_mon + 1;
            sCurrentDateTime.day        = tmDateTime->tm_mday;
            sCurrentDateTime.hour       = tmDateTime->tm_hour;
            sCurrentDateTime.minutes  = tmDateTime->tm_min;
            sCurrentDateTime.seconds  = tmDateTime->tm_sec;
       }
       else
       {
        omxilosalservices::OmxIlOsalDateTime::OmxIlOsalGetDateTime(&sOmxOsalDateTime);
        sCurrentDateTime.year     = sOmxOsalDateTime.year;
        sCurrentDateTime.month    = sOmxOsalDateTime.month;
        sCurrentDateTime.day      = sOmxOsalDateTime.day;
        sCurrentDateTime.hour     = sOmxOsalDateTime.hour;
        sCurrentDateTime.minutes  = sOmxOsalDateTime.minutes;
        sCurrentDateTime.seconds  = sOmxOsalDateTime.seconds;
       }

	 t_sw3A_Buffer* pMakerNotes = p_cam_sm->p3AComponentManager->getPendingMakernotes();
        t_sw3A_Buffer makerNotes;
        if(pMakerNotes == NULL) {
            MSG0("VPB2 Extradata: No makernotes available: pMakerNotes==NULL\n");
            makerNotes.pData = NULL;
            makerNotes.size = 0;
        }
        else {
            makerNotes = *pMakerNotes;
        }
        sExifSettings.pManufacturer        = (const char*)pOmxCamera->mDeviceMakeModel.cMake;
        sExifSettings.pModel               = (const char*)pOmxCamera->mDeviceMakeModel.cModel;
        sExifSettings.sDateTime            = sCurrentDateTime;
        sExifSettings.eOrientation         = OMX_SYMBIAN_OrientationRowTopColumnLeft;//ER: 340986 pOmxCamera->mSceneOrientation.eOrientation;
        sExifSettings.nXResolution         = 72;
        sExifSettings.nYResolution         = 72;
        sExifSettings.eResolutionUnit      = IFM_ResolutionUnit_Inch;
        sExifSettings.eYCbCrPositioning    = IFM_YCbCrPositioning_Centered;
        sExifSettings.nSensitivity         = pCaptureParameters->nSensitivity;
        sExifSettings.eComponentsConfig    = IFM_ComponentsConfiguration_YCbCr;
        sExifSettings.nPictureNumPixelsX   = (OMX_U32)(pPort->getParamPortDefinition().format.video.nStride/pPort->mBytesPerPixel); /* for raw format, nStride/BPP contains proper num of pixels */
        sExifSettings.nPictureNumPixelsY   = (OMX_U32)pPort->getParamPortDefinition().format.video.nSliceHeight; /* should be equal to nFrameHeight, but let's be consistent with previous use of nStride */
        sExifSettings.eWhiteBalanceControl = pOmxCamera->mWhiteBalControl.eWhiteBalControl;
        Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
        sExifSettings.fFocalLength         = ((Cam->mLensParameters.nFocalLength)/65536.0);
        sExifSettings.bPictureEnhanced     = pOmxCamera->mImageFilterType.eImageFilter == OMX_ImageFilterNone ? OMX_FALSE:OMX_TRUE;
        sExifSettings.eSceneType           = pOmxCamera->mSceneMode.eSceneType;
        sExifSettings.eMeteringMode        = pOmxCamera->mExposureValue.eMetering;
        sExifSettings.bFlashFired          = pCaptureParameters->bFlashFired;
        sExifSettings.eFlashControl        = pOmxCamera->mFlashControlStruct.eFlashControl;
        //Note: This boolean value is taken care of in ifm/src/exif.cpp. i.e. the meaning of this bit will be reversed there, to compy with EXIF spec
        sExifSettings.bFlashPresent        = (p_cam_sm->p3AComponentManager->mFlashController.isFlashModeSupported() == true) ? OMX_TRUE: OMX_FALSE;
        sExifSettings.nMaxNumPixelsX       = (OMX_U16)sExifSettings.nPictureNumPixelsX;
        sExifSettings.nExposureTimeUsec    = pCaptureParameters->nExposureTime;
        sExifSettings.xFNumber             = pCaptureParameters->xApertureFNumber;
        sExifSettings.nBrightness          = pOmxCamera->mBrightness.nBrightness;
        sExifSettings.eColorSpace          = IFM_ColorSpace_sRGB;
        sExifSettings.nSubjectDistance     = pOmxCamera->mExtFocusStatus.config.sAFROIs[0].xFocusDistance; // Todo: choose a focused area
        sExifSettings.nEvCompensation      = pOmxCamera->mExposureValue.xEVCompensation;
        sExifSettings.eExposureControl     = pOmxCamera->mExposureControl.eExposureControl;
        sExifSettings.bBracketingEnabled   = (OMX_BOOL)(pOmxCamera->mBracketing.config.nNbrBracketingValues != 0);
        sExifSettings.xAnalogGain          = pCaptureParameters->xAnalogGain;
        sExifSettings.xDigitalGain         = pCaptureParameters->xDigitalGain;
        sExifSettings.nContrast            = pOmxCamera->mContrast.nContrast;
        sExifSettings.sAutoFocusROI        = pOmxCamera->mExtFocusStatus.config.sAFROIs[0].nRect;          // Todo: choose a focused area
        sExifSettings.pMakerNote           = makerNotes.pData;
        sExifSettings.nMakerNoteSize       = makerNotes.size;
	 sExifSettings.pSoftware            = (const char*)pOmxCamera->mSoftwareVersion.cSoftware;
	 sExifSettings.nZoomFactor	= ((p_cam_sm->pZoom->pZoomFactorStatus->xZoomFactor.nValue * 100)/0x10000);	//in Q16 format

        // Add EXIF extradata to current frame

            error = Extradata::SetExtradataField(pOmxBufHdr, &sExifSettings, nVersion);
        if (OMX_ErrorNone != error) {
            MSG0("VPB2: Failed to set the EXIF extradata\n");
        }
		MSG0("VPB2: Set the EXIF extradata\n");

            /* ROIs extradata */
            OMX_SYMBIAN_ROITYPE sROI;
            sROI.nNumberOfROIs = pOmxCamera->mROI.nNumberOfROIs;
            for(OMX_U32 i=0; i<sROI.nNumberOfROIs && i<OMX_SYMBIAN_MAX_NUMBER_OF_ROIS; i++) {
                sROI.sROIs[i] = pOmxCamera->mROI.sROIs[i];
            }

            error = Extradata::SetExtradataField( pOmxBufHdr, &sROI, nVersion);
            if (OMX_ErrorNone != error)
            {
                DBC_ASSERT(0);
            }

            /* Focus Status extradata */
            OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE* pFocusStatusConfig = i3AComponentManager.getFocusStatus(false);
            if (pFocusStatusConfig != NULL) {
                // Structure containing Focus Status extradata
                // Defined as a union so that it contains IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS entries
                union {
                    OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE sExtFocusStatus;
                    OMX_U8 cRawData[sizeof(OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE)- sizeof(OMX_SYMBIAN_AFROITYPE) + IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS*sizeof(OMX_SYMBIAN_AFROITYPE)];
                } sFocusStatusExtradataContainer;

                sFocusStatusExtradataContainer.sExtFocusStatus.nExtraDataVersion = nVersion;
                sFocusStatusExtradataContainer.sExtFocusStatus.nTimestamp        = getTimeElapsedSinceExecutingStateStart();
                sFocusStatusExtradataContainer.sExtFocusStatus.nFrameNum         = pPort->getNumberOfFramesStreamedOut();
                sFocusStatusExtradataContainer.sExtFocusStatus.bFocused          = pFocusStatusConfig->bFocused;
                sFocusStatusExtradataContainer.sExtFocusStatus.nAFAreas          = MIN(pFocusStatusConfig->nAFAreas,IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS);
                for(OMX_U32 i=0; i<sFocusStatusExtradataContainer.sExtFocusStatus.nAFAreas && i<IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS; i++) {
                    sFocusStatusExtradataContainer.sExtFocusStatus.sAFROIs[i] = pFocusStatusConfig->sAFROIs[i];
                }

                error = Extradata::SetExtradataField( pOmxBufHdr,
                                                      (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)&sFocusStatusExtradataContainer,
                                                      sizeof(sFocusStatusExtradataContainer),
                                                      nVersion);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }

            /* Face Tracker Feedback extradata */
            if( pOmxCamera->mAnalyzersFeedbacks.bFaceTrackerFeedbackReceived) {
                // Structure containing Face Tracker analyzer feedback extradata
                // Defined as a union so that it contains one OMX_SYMBIAN_ROITYPE entry
                union {
                    OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE sFeedback;
                    OMX_U8 cRawData[sizeof(OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_ROITYPE)];
                } sFaceTrackerExtradataContainer;

                OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pFaceTrackerFeedback = &pOmxCamera->mAnalyzersFeedbacks.sFaceTracker.sFeedback;

                sFaceTrackerExtradataContainer.sFeedback.nExtraDataVersion    = nVersion;
                sFaceTrackerExtradataContainer.sFeedback.nSize                = pFaceTrackerFeedback->nSize;
                sFaceTrackerExtradataContainer.sFeedback.nOriginatorTimestamp = pFaceTrackerFeedback->nOriginatorTimestamp;
                sFaceTrackerExtradataContainer.sFeedback.nOriginatorFrameNum  = pFaceTrackerFeedback->nOriginatorFrameNum;
                sFaceTrackerExtradataContainer.sFeedback.nFeedbackType        = pFaceTrackerFeedback->nFeedbackType;
                sFaceTrackerExtradataContainer.sFeedback.nFeedbackValueSize   = pFaceTrackerFeedback->nFeedbackValueSize;
                *(OMX_SYMBIAN_ROITYPE*)sFaceTrackerExtradataContainer.sFeedback.nFeedbackValue = *(OMX_SYMBIAN_ROITYPE*)pFaceTrackerFeedback->nFeedbackValue;

                error = Extradata::SetExtradataField( pOmxBufHdr,
                                                      (OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE*)&sFaceTrackerExtradataContainer,
                                                      sFaceTrackerExtradataContainer.sFeedback.nFeedbackType,
                                                      sizeof(sFaceTrackerExtradataContainer),
                                                      nVersion);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }

            /* Object Tracker Feedback extradata */
            if( pOmxCamera->mAnalyzersFeedbacks.bObjectTrackerFeedbackReceived) {
                // Structure containing Object Tracker analyzer feedback extradata
                // Defined as a union so that it contains one OMX_SYMBIAN_ROITYPE entry
                union {
                    OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE sFeedback;
                    OMX_U8 cRawData[sizeof(OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_ROITYPE)];
                } sObjectTrackerExtradataContainer;

                OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pObjectTrackerFeedback = &pOmxCamera->mAnalyzersFeedbacks.sObjectTracker.sFeedback;

                sObjectTrackerExtradataContainer.sFeedback.nExtraDataVersion    = nVersion;
                sObjectTrackerExtradataContainer.sFeedback.nSize                = pObjectTrackerFeedback->nSize;
                sObjectTrackerExtradataContainer.sFeedback.nOriginatorTimestamp = pObjectTrackerFeedback->nOriginatorTimestamp;
                sObjectTrackerExtradataContainer.sFeedback.nOriginatorFrameNum  = pObjectTrackerFeedback->nOriginatorFrameNum;
                sObjectTrackerExtradataContainer.sFeedback.nFeedbackType        = pObjectTrackerFeedback->nFeedbackType;
                sObjectTrackerExtradataContainer.sFeedback.nFeedbackValueSize   = pObjectTrackerFeedback->nFeedbackValueSize;
                *(OMX_SYMBIAN_ROITYPE*)sObjectTrackerExtradataContainer.sFeedback.nFeedbackValue = *(OMX_SYMBIAN_ROITYPE*)pObjectTrackerFeedback->nFeedbackValue;

                error = Extradata::SetExtradataField( pOmxBufHdr,
                                                      (OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE*)&sObjectTrackerExtradataContainer,
                                                      sObjectTrackerExtradataContainer.sFeedback.nFeedbackType,
                                                      sizeof(sObjectTrackerExtradataContainer),
                                                      nVersion);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }

            /* Motion Level Feedback extradata */
            if( pOmxCamera->mAnalyzersFeedbacks.bMotionLevelFeedbackReceived) {
                // Structure containing Motion Level analyzer feedback extradata
                // Defined as a union so that it contains one OMX_U32 entry
                union {
                    OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE sFeedback;
                    OMX_U8 cRawData[sizeof(OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE) - sizeof(OMX_U8) + sizeof(OMX_U32)];
                } sMotionLevelExtradataContainer;

                OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pMotionLevelFeedback = &pOmxCamera->mAnalyzersFeedbacks.sMotionLevel.sFeedback;

                sMotionLevelExtradataContainer.sFeedback.nExtraDataVersion    = nVersion;
                sMotionLevelExtradataContainer.sFeedback.nSize                = pMotionLevelFeedback->nSize;
                sMotionLevelExtradataContainer.sFeedback.nOriginatorTimestamp = pMotionLevelFeedback->nOriginatorTimestamp;
                sMotionLevelExtradataContainer.sFeedback.nOriginatorFrameNum  = pMotionLevelFeedback->nOriginatorFrameNum;
                sMotionLevelExtradataContainer.sFeedback.nFeedbackType        = pMotionLevelFeedback->nFeedbackType;
                sMotionLevelExtradataContainer.sFeedback.nFeedbackValueSize   = pMotionLevelFeedback->nFeedbackValueSize;
                *(OMX_U32*)sMotionLevelExtradataContainer.sFeedback.nFeedbackValue = *(OMX_U32*)pMotionLevelFeedback->nFeedbackValue;

                error = Extradata::SetExtradataField( pOmxBufHdr,
                                                      (OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE*)&sMotionLevelExtradataContainer,
                                                      sMotionLevelExtradataContainer.sFeedback.nFeedbackType,
                                                      sizeof(sMotionLevelExtradataContainer),
                                                      nVersion);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }

            /* Scene Detection Feedback extradata */
            if( pOmxCamera->mAnalyzersFeedbacks.bSceneDetectionFeedbackReceived) {
                // Structure containing Scene Detection analyzer feedback extradata
                // Defined as a union so that it contains one OMX_SYMBIAN_SCENEMODETYPE entry
                union {
                    OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE sFeedback;
                    OMX_U8 cRawData[sizeof(OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_SCENEMODETYPE)];
                } sSceneDetectionExtradataContainer;

                OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pSceneDetectionFeedback = &pOmxCamera->mAnalyzersFeedbacks.sSceneDetection.sFeedback;

                sSceneDetectionExtradataContainer.sFeedback.nExtraDataVersion    = nVersion;
                sSceneDetectionExtradataContainer.sFeedback.nSize                = pSceneDetectionFeedback->nSize;
                sSceneDetectionExtradataContainer.sFeedback.nOriginatorTimestamp = pSceneDetectionFeedback->nOriginatorTimestamp;
                sSceneDetectionExtradataContainer.sFeedback.nOriginatorFrameNum  = pSceneDetectionFeedback->nOriginatorFrameNum;
                sSceneDetectionExtradataContainer.sFeedback.nFeedbackType        = pSceneDetectionFeedback->nFeedbackType;
                sSceneDetectionExtradataContainer.sFeedback.nFeedbackValueSize   = pSceneDetectionFeedback->nFeedbackValueSize;
                *(OMX_SYMBIAN_SCENEMODETYPE*)sSceneDetectionExtradataContainer.sFeedback.nFeedbackValue = *(OMX_SYMBIAN_SCENEMODETYPE*)pSceneDetectionFeedback->nFeedbackValue;

                error = Extradata::SetExtradataField( pOmxBufHdr,
                                                      (OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE*)&sSceneDetectionExtradataContainer,
                                                      sSceneDetectionExtradataContainer.sFeedback.nFeedbackType,
                                                      sizeof(sSceneDetectionExtradataContainer),
                                                      nVersion);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }

            // Push frame out of port
            mENSComponent.fillBufferDone(pOmxBufHdr);

        } else {
            // Frame skipped: recycle buffer
            fillThisBuffer(pOmxBufHdr);
        }
    } else {
        // Forwards all frames to the ENS so that the port can be properly flushed before going to Idle
        mENSComponent.fillBufferDone(pOmxBufHdr);
    }

    ASYNC_OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::fillBufferDoneVPB2");
}



/* */
/* Camera::fillBufferDoneVPB0 : Viewfinder call-back (StillPreview/VideoPreview)
 */
/**/
void Camera::fillBufferDoneVPB0(OMX_BUFFERHEADERTYPE* pOmxBufHdr) {
    ASYNC_IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::fillBufferDoneVPB0");
    camport* pPort = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT0);

	 if(p_cam_sm->StillinVideo==1)
		pOmxBufHdr->nFlags  &= ~ OMX_BUFFERFLAG_EOS;
   
    if((pOmxBufHdr->nFlags & OMX_BUFFERFLAG_EOS) != 0) {

    	//performance traces start
    	if(1 == latency_ShotToVF)
    	{
    		p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
    		mlatency_Hsmcamera.e_OMXCAM_ShotToVF.t0 = p_cam_sm->mTime;
    		latency_ShotToVF = 0;
    		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToVF %d", 0);
    	}
    	//performance traces end

        /* we never want to forward the EOS flag on VPB0, remove it */
        pOmxBufHdr->nFlags &= ~OMX_BUFFERFLAG_EOS;

    }

	if(0 != pOmxBufHdr->nFilledLen) {
		//performance traces start
		p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
		mlatency_Hsmcamera.e_OMXCAM_VPB0FrameRate.t0 = p_cam_sm->mTime;
		mlatency_Hsmcamera.e_OMXCAM_VPB0FrameRate.t1 = p_cam_sm->mTime;
		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_VPB0FrameRate %d", 0);
		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_VPB0FrameRate %d", 1);
		//performance traces end
	}

#ifdef FACE_DETECTOR_TEST_DRAWOBJECTS_ON
    //---------------------------------------------------------------------
    // For test purpose, we draw ROIs in the buffer.
    //---------------------------------------------------------------------
    static const int MAX_DRAWOBJECTS=OMX_SYMBIAN_MAX_NUMBER_OF_ROIS;
    imgTest_drawObject a_Obj[MAX_DRAWOBJECTS];
    int objectCount=0;

    COmxCamera* omxcam = (COmxCamera *)&mENSComponent;
    OMX_SYMBIAN_CONFIG_ROITYPE* pROI = &(omxcam->mROI);

    for (unsigned int i=0; i<omxcam->mROI.nNumberOfROIs && objectCount<MAX_DRAWOBJECTS; i++) {
        if ((pROI->sROIs[i].sROI.sReference.nWidth ==0) || (pROI->sROIs[i].sROI.sReference.nHeight ==0)) {
            MSG0("Warning: Null ROI reference size. Skipping ROI drawing.\n");
            OstTraceFiltInst0(TRACE_WARNING, "Warning: Null ROI reference size. Skipping ROI drawing.");
            continue;
        }
        a_Obj[objectCount].type    = IMGTEST_DRAWOBJECT_RECT;
        a_Obj[objectCount].RGBColor=0x00FF88;
        a_Obj[objectCount].coords.rect.x0 = (pROI->sROIs[i].sROI.sRect.sTopLeft.nX * 100000) / pROI->sROIs[i].sROI.sReference.nWidth;
        a_Obj[objectCount].coords.rect.y0 = (pROI->sROIs[i].sROI.sRect.sTopLeft.nY * 100000) / pROI->sROIs[i].sROI.sReference.nHeight;
        a_Obj[objectCount].coords.rect.x1 = ( (pROI->sROIs[i].sROI.sRect.sTopLeft.nX + pROI->sROIs[i].sROI.sRect.sSize.nWidth)* 100000 ) / pROI->sROIs[i].sROI.sReference.nWidth;
        a_Obj[objectCount].coords.rect.y1 = ( (pROI->sROIs[i].sROI.sRect.sTopLeft.nY + pROI->sROIs[i].sROI.sRect.sSize.nHeight)* 100000 ) / pROI->sROIs[i].sROI.sReference.nHeight;
        objectCount++;
    }

    e_imgTestC_colorFormat colorFmt = IMGTEST_DRAWCOLORFMT_NONE;
    int bpp = 0;
    camport * curPort = (camport *)  mENSComponent.getPort(CAMERA_PORT_OUT0);
    switch(curPort->getParamPortDefinition().format.video.eColorFormat) {
        case OMX_COLOR_Format16bitRGB565:
            colorFmt = IMGTEST_DRAWCOLORFMT_RGB565;
            bpp = 2;
            break;
        case OMX_COLOR_Format24bitRGB888:
            colorFmt = IMGTEST_DRAWCOLORFMT_RGB888;
            bpp = 3;
            break;
        case OMX_COLOR_FormatCbYCrY:
            colorFmt = IMGTEST_DRAWCOLORFMT_CbYCrY422;
            bpp = 2;
            break;
        default:
            ; // Color format is not supported by the drawer. Skip drawing.
    }

    if (colorFmt != IMGTEST_DRAWCOLORFMT_NONE) {
        imgTest_draw(pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset,
                     curPort->getPictureWidth(),
                     curPort->getPictureHeight(),
                     curPort->getPictureWidth() * bpp,
                     colorFmt,
                     a_Obj, objectCount);
    }
#endif /* FACE_DETECTOR_TEST_DRAWOBJECTS_ON */

    // Recycle the buffer if it should not be passed to the next OMX component.
    OMX_STATETYPE state = pPort->getPortFsm()->getState();

    if ((state == OMX_StateExecuting) && pPort->isEnabled()==OMX_TRUE)
    {
        // In Executing skip needed frames, according to framerate constraints.
        pPort->incrementNumberOfFramesGrabbed();
        if ((pPort->getNumberOfFramesGrabbed() % iFramerate.getFrameSkipRatio(0)) == 0) {

            /* Frame to be pushed out of port */
            OMX_ERRORTYPE error = OMX_ErrorNone;
            OMX_VERSIONTYPE version;
            version.nVersion = OMX_VERSION;

            error = Extradata::SetExtradataField(pOmxBufHdr, &(((COmxCamera*)&mENSComponent)->mCameraUserSettings), version);
            if (OMX_ErrorNone != error)
            {
                DBC_ASSERT(0);
            }

            OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pCaptureParameters = i3AComponentManager.getCaptureParameters(false);
            if (pCaptureParameters != NULL) {
                error = Extradata::SetExtradataField(pOmxBufHdr, pCaptureParameters, version);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }
            
            OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraCaptureParameters = i3AComponentManager.getSteExtraCaptureParameters(false);
            if (pSteExtraCaptureParameters != NULL) {
                error = Extradata::SetExtradataField(pOmxBufHdr, pSteExtraCaptureParameters, version);
                if (OMX_ErrorNone != error)
                {
                    DBC_ASSERT(0);
                }
            }

            // Done for flash production test
            if(pPort->bOneShot==OMX_FALSE)
            {
                t_sw3A_Buffer* buff = i3AComponentManager.getFlashStatusData();
                if(buff!=NULL && buff->size!=0 && buff->pData!=NULL)
                {
                    OMX_STE_PRODUCTIONTESTTYPE *data = (OMX_STE_PRODUCTIONTESTTYPE*) buff->pData;
                    data->nVersion = version;
                    data->nSize=buff->size;
                    error = Extradata::SetExtradataField(pOmxBufHdr,data,version);
                    if (OMX_ErrorNone != error)
                    {
                        DBC_ASSERT(0);
                    }
                }
            }

            // Push frame out of port
            mENSComponent.fillBufferDone(pOmxBufHdr);

        } else {
            // Frame skipped: recycle buffer
            fillThisBuffer(pOmxBufHdr);
        }
    } else {
        // Forwards all frames to the ENS so that the port can be properly flushed before going to Idle
        mENSComponent.fillBufferDone(pOmxBufHdr);
    }

    ASYNC_OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::fillBufferDoneVPB0");
}

#ifdef CAMERA_ENABLE_OMX_STATS_PORT
static OMX_ERRORTYPE statsCallback(void* userData, OMX_BUFFERHEADERTYPE* pBuffer)
{
  OMX_ERRORTYPE omxErr = OMX_ErrorNone;
  Camera* pCam = (Camera*)userData;
  pCam->fillBufferDone(pBuffer);
  return omxErr;
}
void Camera::fillBufferDoneStatsAewb(OMX_BUFFERHEADERTYPE *pOMXBuffer)
{
  mENSComponent.fillBufferDone(pOMXBuffer);
}
void Camera::fillBufferDoneStatsAf(OMX_BUFFERHEADERTYPE *pOMXBuffer)
{
  mENSComponent.fillBufferDone(pOMXBuffer);
}
#endif // CAMERA_ENABLE_OMX_STATS_PORT

/* */
/* overwrites fillthisbuffer to count the number of buffers at MPC
 */
/**/
OMX_ERRORTYPE Camera::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer) {
    //IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::fillThisBuffer");

    if(iPanic==OMX_TRUE)return OMX_ErrorHardware;
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
    if (pBuffer->nOutputPortIndex == CAMERA_PORT_STATS_AEWB ||
        pBuffer->nOutputPortIndex == CAMERA_PORT_STATS_AF)
    {
      MSG2("FillThisBuffer from client port = %ld, pBufer = %p\n", pBuffer->nOutputPortIndex, pBuffer);
      return pExtItf->FillThisBuffer(pExtItf->handler, pBuffer);
    }
#endif // CAMERA_ENABLE_OMX_STATS_PORT

    if (CAMERA_PORT_OUT0 == pBuffer->nOutputPortIndex)
    {
    	//performance traces start
        if ((0 != latency_ShotToSnapshotStart) && (OMX_FALSE == iOpModeMgr.ConfigCapturing[CAMERA_PORT_OUT1]))
        {
        	p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
        	mlatency_Hsmcamera.e_OMXCAM_ShotToSnapshotStart.t0 = p_cam_sm->mTime;
        	OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToSnapshot %d", 0);
            latency_ShotToSnapshotStart = 0;
        }

        if (0 != latency_VFOffToOnStart)
        {
        	p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
        	mlatency_Hsmcamera.e_OMXCAM_VFOffToOn.t0 = p_cam_sm->mTime;
        	OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_VFOffToOn %d", 0);
            latency_VFOffToOnStart = 0;
        }
        //performance traces end
    }

    iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]++;
    MSG2("FTB iOpModeMgr.nBufferAtMPC[%ld] = %d\n",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
    OstTraceFiltInst2(TRACE_DEBUG, "FTB iOpModeMgr.nBufferAtMPC[%ld] = %d",pBuffer->nOutputPortIndex,iOpModeMgr.nBufferAtMPC[pBuffer->nOutputPortIndex]);
    mIfillThisBuffer[pBuffer->nOutputPortIndex].fillThisBuffer(pBuffer);

    /* notify state machine that a buffer is now available */
    if(((CAMERA_PORT_OUT0 == pBuffer->nOutputPortIndex)
            && (OMX_TRUE == iOpModeMgr.nWaitingForBuffer[CAMERA_PORT_OUT0]))
        ||((CAMERA_PORT_OUT1 == pBuffer->nOutputPortIndex)
            && (OMX_TRUE == iOpModeMgr.nWaitingForBuffer[CAMERA_PORT_OUT1]))
        ||((CAMERA_PORT_OUT2 == pBuffer->nOutputPortIndex)
            && (OMX_TRUE == iOpModeMgr.nWaitingForBuffer[CAMERA_PORT_OUT2])))
    {
        iOpModeMgr.nWaitingForBuffer[CAMERA_PORT_OUT0] = OMX_FALSE;
        iOpModeMgr.nWaitingForBuffer[CAMERA_PORT_OUT1] = OMX_FALSE;
        iOpModeMgr.nWaitingForBuffer[CAMERA_PORT_OUT2] = OMX_FALSE;

        s_scf_event devent;
        devent.sig = BUFFER_AVAILABLE_SIG;
        p_cam_sm->ProcessEvent(&devent);
    }

    //OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::fillThisBuffer");
    return OMX_ErrorNone;

}


/* */
/* Clock Callacks : obsolete
 */
/**/
void Camera::clockrunning(t_uint16 port_idx){
    ASYNC_IN0("\n");
    ASYNC_OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::clockrunning");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::clockrunning");
}

void Camera::clockwaitingforstart(t_uint16 port_idx){
    ASYNC_IN0("\n");
    ASYNC_OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::clockwaitingforstart");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::clockwaitingforstart");
}

void Camera::configured(t_uint16 port_idx){
    ASYNC_IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::configured");

    s_scf_event devent;
    devent.sig = CONFIGURE_SIG;
    devent.type.other = port_idx;
    p_cam_sm->ProcessEvent(&devent);

    ASYNC_OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::configured");
}

void Camera::bufferGrabbed(t_uint16 port_idx){
    ASYNC_IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::bufferGrabbed");

    s_scf_event event;
    event.type.other=0;
    event.sig = CAMERA_BUFFERED_SIG;
    iDeferredEventMgr.queuePriorEvent(&event);

    s_scf_event devent;
    devent.sig = Q_DEFERRED_EVENT_SIG;
    p_cam_sm->ProcessEvent(&devent);

    ASYNC_OUT0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::bufferGrabbed");
}

/* */
/* grab_api_alert callbacks
 */
/**/
void Camera::info(enum e_grabInfo info_id, t_uint16 buffer_id, t_uint16 frame_id, t_uint32 timestamp)
{
	ASYNC_IN0("\n");
	MSG1("\t Camera::info (grab callback) : %d ", info_id);
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::info");
	OstTraceFiltInst1(TRACE_DEBUG, "\t Camera::info (grab callback) : %d ", info_id);
	if ((OMX_TRUE == iSelfTest.pTesting->bEnabled) && (GRBINF_GRAB_COMPLETED == info_id))
	{
		iSelfTest.selftestError = OMX_ErrorNone;
        s_scf_event event;
        event.type.other=0;
		event.sig = CAMERA_INDEX_CONFIG_SELFTEST_SIG;
		iDeferredEventMgr.queuePriorEvent(&event);
		s_scf_event devent;
		devent.sig = Q_DEFERRED_EVENT_SIG;
		p_cam_sm->ProcessEvent(&devent);
	}
	ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::info");
}

void Camera::error(enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp)
{
	ASYNC_IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::error");
	if (OMX_TRUE == iSelfTest.pTesting->bEnabled)
	{
		MSG1("\t Camera::error (grab callback) : %d \n", error_id);
		OstTraceFiltInst1(TRACE_DEBUG, "\t Camera::error (grab callback) : %d ", error_id);
		iSelfTest.selftestError = OMX_ErrorHardware;
        s_scf_event event;
        event.type.other=0;
		event.sig = CAMERA_INDEX_CONFIG_SELFTEST_SIG;
		iDeferredEventMgr.queuePriorEvent(&event);
		s_scf_event devent;
		devent.sig = Q_DEFERRED_EVENT_SIG;
		p_cam_sm->ProcessEvent(&devent);
	}
	ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::error");
}

/* */
/* Only the FF errors are asserted right now
 * TODO: Implement error management
 */
/**/
void Camera::error(enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id) {

    ASYNC_IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::error");

    MSG3("grabctl error: VPB%x error_id=0x%X data=0x%X\n", pipe_id, error_id, data);
    OstTraceFiltInst3(TRACE_DEBUG, "grabctl error: VPB%x error_id=0x%X data=0x%X", pipe_id, error_id, data);

    //Save grab error in context information
    grabErrorDetail[pipe_id].error_id = error_id;
    grabErrorDetail[pipe_id].data = data;

    /* TODO: handle errors :p */
    if(
        (((data==IPP_SD_ERROR_PIPE0_CH0_OVERFLOW)||(data==IPP_SD_ERROR_PIPE0_CH1_OVERFLOW)||(data==IPP_SD_ERROR_PIPE0_CH2_OVERFLOW))&&(error_id==IPP_SD_ERROR_HR))||
        ((data==IPP_SD_ERROR_PIPE1_OVERFLOW)&&(error_id==IPP_SD_ERROR_LR))||
        ((data==IPP_SD_ERROR_PIPE2_OVERFLOW)&&(error_id==IPP_SD_ERROR_BMS))
    )
    {
        MSG2("Unhandled hardware error 0x%X (data 0x%X)\n", error_id, data);
        OstTraceFiltInst2(TRACE_DEBUG, "Unhandled hardware error 0x%X (data 0x%X)", error_id, data);
        // do not ASSERT since error recovery on dsp is working
        // DBC_ASSERT(0);
    }



    ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::error");
}

void Camera::debug(enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp)
{
	ASYNC_IN0("\n");
	ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::debug");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::debug");
}


/* */
/* timer_api_alarm callbacks
 */
/**/
void Camera::signal(void)
{
	ASYNC_IN0("\n");
	MSG0("Camera::signal !!!!!\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::signal");
	OstTraceFiltInst0(TRACE_DEBUG, "Camera::signal !!!!!");

	if( NULL == p_cam_sm )
	{
		MSG0("	p_cam_sm already destroyed.... \n");
		OstTraceFiltInst0(TRACE_DEBUG, "	p_cam_sm already destroyed.... ");
		return;
	}

	if (OMX_TRUE == p_cam_sm->pSelfTest->pTesting->bEnabled){
	    s_scf_event devent;
	    devent.sig = TIMEOUT_SIG;
	    p_cam_sm->ProcessEvent(&devent);
        }

#ifdef CHECK_MMDSP_ALIVE
  /* debug hook to check if MMDSP is still alive and able to do an __XIO access.
   This command will read SIA_CLK_ENABLE register, triggering a Debug callback
   in imgcommon (Callback_ispctlDebug message) */
  MSG0("Try to read HW register SIA_CLK_ENABLE through MMDSP\n");
	OstTraceFiltInst0(TRACE_DEBUG, "Try to read HW register SIA_CLK_ENABLE through MMDSP");
  iIspctlComponentManager.getIspctlDebug().readRegister(0xA008U);
#endif

	if (iCheckAliveTimerRunning) {
	    iNbAliveIterval500Msec++;
            if (iCheckIntervalMsec > 0 && iPrintStatus){
                if (iNbAliveIterval500Msec == iCheckIntervalMsec/500){
                    printCheckAliveStatus();
                    iNbAliveIterval500Msec=0;
                }
            }
        }
	if (iCheckTimerRunning){
	    iNbLowPowerInterval500Msec++;
	    if (iNbLowPowerInterval500Msec > 5){
                s_scf_event event;
                event.type.other=0;
	        event.sig = LOWPOWER_SIG;
	        iDeferredEventMgr.queueNewEvent(&event);
	        s_scf_event devent;
	        devent.sig = Q_DEFERRED_EVENT_SIG;
                p_cam_sm->ProcessEvent(&devent);
                iNbLowPowerInterval500Msec=0;
                iCheckTimerRunning = false;
             }
        }
	for(int i =0; i<TT_MAX ; i++)
	{

		if (p_cam_sm->iTimed_Task[i].timer_running == TRUE)
		{

			(p_cam_sm->iTimed_Task[i].current_time)++;
			MSG2("\niTimed_Task[%d].current_time = %d\n",i,p_cam_sm->iTimed_Task[i].current_time);
			if((p_cam_sm->iTimed_Task[i].current_time * 1000) >= p_cam_sm->iTimed_Task[i].timeout)
			{
#ifdef CAM_STILL_SYNCHRO_DBG
				printf("---timer%dexpired----",i);
#endif
				MSG1("---timer%dexpired----",i);
				p_cam_sm->iTimed_Task[i].current_time = 0;
				p_cam_sm->iTimed_Task[i].timer_running = FALSE;//stop timer
				mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorTimeout, i);
			}
		}
	}

	ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::signal");
}


/* */
/* asyncflash_api_response callback
 */
/**/
void Camera::asyncFlashSelfTestResult(t_sint32 aErr)
{
	ASYNC_IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::asyncFlashSelfTestResult");
	if (FLASH_ERR_NONE != aErr)
	{
		iSelfTest.selftestError = OMX_ErrorHardware;
	}
	else
	{
		iSelfTest.selftestError = OMX_ErrorNone;
	}
	s_scf_event event;
	event.type.other=0;
	event.sig = CAMERA_INDEX_CONFIG_SELFTEST_SIG;
	iDeferredEventMgr.queuePriorEvent(&event);
	s_scf_event devent;
	devent.sig = Q_DEFERRED_EVENT_SIG;
	p_cam_sm->ProcessEvent(&devent);
	ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::asyncFlashSelfTestResult");
}

void Camera::asyncFlashReturnCode(t_sint32 aErr)
{
	ASYNC_IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::asyncFlashReturnCode");
	s_scf_event event;
	if( aErr == FLASH_RET_NONE )
	{
		event.sig = ASYNC_FLASH_OPERATION_DONE_SIG;
	}
	else
	{
		event.sig = ASYNC_FLASH_OPERATION_FAILED_SIG;
	}
	event.type.other = 0;
	p_cam_sm->ProcessEvent(&event);
	ASYNC_OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::asyncFlashReturnCode");
}


/* */
/* doSpecificEventHandler_cb is used in case where the Camera has to dispatch/delay the callback handler
*/
OMX_ERRORTYPE Camera::doSpecificEventHandler_cb(OMX_EVENTTYPE ev, OMX_U32 data1, OMX_U32 data2,OMX_BOOL & bDeferredEventHandler) {

    IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::doSpecificEventHandler_cb");
    OMX_STATETYPE exitedState = OMX_StateInvalid;
    mENSComponent.GetState(&mENSComponent, &exitedState);

    s_scf_event event;
    bDeferredEventHandler = OMX_FALSE;
    switch(ev){
        case OMX_EventCmdComplete:
            {
                if(data1 == OMX_CommandStateSet)
                {

					if (data2 == OMX_StateIdle && ((exitedState == OMX_StateExecuting) || (exitedState == OMX_StatePause)))
					{
						MSG2("OMX_CommandStateSet : exitedState=%d(%s), currentState=%ld (%s)\n", exitedState,
							CError::stringOMXstate(exitedState),data2,CError::stringOMXstate((OMX_STATETYPE)data2));
						OstTraceFiltInst2(TRACE_DEBUG, "OMX_CommandStateSet : exitedState=%d, currentState=%ld", exitedState,data2);
						event.type.other=0;
						event.sig = CAMERA_EXECUTING_TO_IDLE_END_SIG;
						iDeferredEventMgr.queueNewEvent(&event);
						s_scf_event devent;
						devent.sig = Q_DEFERRED_EVENT_SIG;
						p_cam_sm->ProcessEvent(&devent);
						bDeferredEventHandler = OMX_TRUE;
					}
					else if (data2 == OMX_StateIdle && exitedState == OMX_StateLoaded)
					{

						MSG2("OMX_CommandStateSet : exitedState=%d(%s), currentState=%ld (%s)\n", exitedState,
							CError::stringOMXstate(exitedState),data2,CError::stringOMXstate((OMX_STATETYPE)data2));
						OstTraceFiltInst2(TRACE_DEBUG, "OMX_CommandStateSet : exitedState=%d, currentState=%ld", exitedState,data2);
						constructSM(mENSComponent);
#ifdef MS_IMP
                        p_cam_sm->init(this); // the CB will be sent after the init sequence.
#elif defined(EMPTY_IMP) || defined(PR_IMP)
                        // JMV: We initialize the camera state machine.
                        SCF_START_SM(p_cam_sm);
#else
    #error "implementation missing"
#endif

						bDeferredEventHandler = OMX_TRUE;
						p_cam_sm->p3AComponentManager = &i3AComponentManager;
					}
					else if (((data2 == OMX_StateExecuting) || (data2 == OMX_StatePause)) && exitedState == OMX_StateIdle)
					{
						MSG2("OMX_CommandStateSet : exitedState=%d(%s), currentState=%ld (%s)\n", exitedState,
							CError::stringOMXstate(exitedState),data2,CError::stringOMXstate((OMX_STATETYPE)data2));
						OstTraceFiltInst2(TRACE_DEBUG, "OMX_CommandStateSet : exitedState=%d, currentState=%ld", exitedState,data2);
						if (data2 == OMX_StateExecuting) {
							//performance traces start
							p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
							mlatency_Hsmcamera.e_OMXCAM_VFOffToOn.t1 = p_cam_sm->mTime;
							OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_VFOffToOn %d", 1);
							latency_VFOffToOnStart = 1;
							//performance traces end
						}
						event.type.other=0;
						event.sig = CAMERA_IDLE_TO_EXECUTING_SIG;
						event.args.sendCommand.nParam = data2;
						event.args.sendCommand.nSendCallback = OMX_TRUE;
						iDeferredEventMgr.queueNewEvent(&event);
						s_scf_event devent;
						devent.sig = Q_DEFERRED_EVENT_SIG;
						p_cam_sm->ProcessEvent(&devent);
						bDeferredEventHandler = OMX_TRUE;
					}
					else if ( (data2 == OMX_StatePause) && exitedState == OMX_StateExecuting){
						MSG2("OMX_CommandStateSet : exitedState=%d(%s), currentState=%ld (%s)\n", exitedState,
							CError::stringOMXstate(exitedState),data2,CError::stringOMXstate((OMX_STATETYPE)data2));
						OstTraceFiltInst2(TRACE_DEBUG, "OMX_CommandStateSet : exitedState=%d, currentState=%ld", exitedState,data2);
						event.type.other=0;
						event.sig = CAMERA_EXECUTING_TO_PAUSE_SIG;
						iDeferredEventMgr.queueNewEvent(&event);
						s_scf_event devent;
						devent.sig = Q_DEFERRED_EVENT_SIG;
						p_cam_sm->ProcessEvent(&devent);
						bDeferredEventHandler = OMX_TRUE;
					}
					else if ( (data2 == OMX_StateExecuting) && exitedState == OMX_StatePause){
						MSG2("OMX_CommandStateSet : exitedState=%d(%s), currentState=%ld (%s)\n", exitedState,
							CError::stringOMXstate(exitedState),data2,CError::stringOMXstate((OMX_STATETYPE)data2));
						OstTraceFiltInst2(TRACE_DEBUG, "OMX_CommandStateSet : exitedState=%d, currentState=%ld", exitedState,data2);
						event.type.other=0;
						event.sig = CAMERA_PAUSE_TO_EXECUTING_SIG;
						iDeferredEventMgr.queueNewEvent(&event);
						s_scf_event devent;
						devent.sig = Q_DEFERRED_EVENT_SIG;
						p_cam_sm->ProcessEvent(&devent);
						bDeferredEventHandler = OMX_TRUE;
					}

				}
				else if (data1 == OMX_CommandPortEnable)
				{
					MSG2("OMX_CommandPortEnable : in State=%d(%s)\n", exitedState,CError::stringOMXstate(exitedState));
					OstTraceFiltInst1(TRACE_DEBUG, "OMX_CommandPortEnable : exitedState=%d", exitedState);
					if((exitedState == OMX_StateIdle)||(exitedState == OMX_StateExecuting))
					{
						bDeferredEventHandler = OMX_FALSE;
					}
				}
			}
		break;
	case OMX_EventError:
	{
	    MSG2("OMX_EventError: data1=%lu data2=%lu\n", data1, data2);
	    OstTraceFiltInst2(TRACE_DEBUG, "OMX_EventError: data1=%lu data2=%lu", data1, data2);
	    break;
	}

        default: break;
    }
    OUT0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::doSpecificEventHandler_cb");
    return OMX_ErrorNone;
}



/*
 * Construct CAM_SM
 */
void Camera::constructSM(ENS_Component &enscomp){

    MSG0("SM from Camera is initialized and started\n");
	OstTraceFiltInst0(TRACE_DEBUG, "SM from Camera is initialized and started");
    p_cam_sm = new CAM_SM(enscomp);

    p_cam_sm->RecordPort = RecordPort;
    p_cam_sm->CapturePort = CapturePort;
    p_cam_sm->RawCapturePort = RawCapturePort;
    p_cam_sm->pIspctlCom = &iIspctlCom;
    p_cam_sm->pDeferredEventMgr =&iDeferredEventMgr ;
    p_cam_sm->pOmxStateMgr =&iOmxStateMgr ;
    p_cam_sm->pTrace = &iTrace;
    p_cam_sm->pOpModeMgr = &iOpModeMgr;
    p_cam_sm->pIspctlComponentManager = &iIspctlComponentManager;
    p_cam_sm->pGrabControl = &iGrabControl;
    p_cam_sm->pGrab = &iGrab;
    p_cam_sm->pCapture_context = &iCapture_context;
    p_cam_sm->pZoom = &iZoom;
    p_cam_sm->pTestMode = &iTestMode;
    p_cam_sm->pTuningDataBase = pTuningDataBase;
    p_cam_sm->pIspDampers = &iIspDampers;
    p_cam_sm->pPictureSettings = &iPictureSettings;
    p_cam_sm->pSensor = iSensor;
    p_cam_sm->pSharedMemory = &iSharedMemory;
    p_cam_sm->pSensorModesShmChunk = &iSensorModesShmChunk;
    p_cam_sm->pNvmShmChunk = &iNvmShmChunk;
    p_cam_sm->pFwShmChunk = &iFwShmChunk;
    p_cam_sm->pResourceSharerManager=&iResourceSharerManager;
    p_cam_sm->pSelfTest = &iSelfTest;
	/*For Gamma*/
    p_cam_sm->pGammaSharpGreen_LR= &iGammaSharpGreen_LR;
    p_cam_sm->pGammaSharpRed_LR = &iGammaSharpRed_LR;
    p_cam_sm->pGammaSharpBlue_LR= &iGammaSharpBlue_LR;
    p_cam_sm->pGammaUnSharpGreen_LR = &iGammaUnSharpGreen_LR;
    p_cam_sm->pGammaUnSharpRed_LR = &iGammaUnSharpRed_LR;
    p_cam_sm->pGammaUnSharpBlue_LR = &iGammaUnSharpBlue_LR;

    p_cam_sm->pGammaSharpGreen_HR = &iGammaSharpGreen_HR;
    p_cam_sm->pGammaSharpRed_HR = &iGammaSharpRed_HR;
    p_cam_sm->pGammaSharpBlue_HR = &iGammaSharpBlue_HR;
    p_cam_sm->pGammaUnSharpGreen_HR = &iGammaUnSharpGreen_HR;
    p_cam_sm->pGammaUnSharpRed_HR = &iGammaUnSharpRed_HR;
    p_cam_sm->pGammaUnSharpBlue_HR = &iGammaUnSharpBlue_HR;
	/*For Shutterlag */
    p_cam_sm->Sw3aStopReqStatus = Sw3aStopReqStatus;
    p_cam_sm->GrabAbortStatus = GrabAbortStatus;

    i3AComponentManager.setSM(p_cam_sm);
    i3AComponentManager.setDeferredEventManager(&iDeferredEventMgr);
    i3AComponentManager.setIspCtlClbks(&iIspctlComponentManager.mEventClbks);
    i3AComponentManager.setSharedMem(&iSharedMemory);
    p_cam_sm->p3AComponentManager = &i3AComponentManager;
    p_cam_sm->pFlashDriver = iFlashDriver;
    p_cam_sm->pFramerate = &iFramerate;

    iTuningLoaderManager.setSM(p_cam_sm);
    p_cam_sm->pTuningLoaderManager = &iTuningLoaderManager;

    iIspctlComponentManager.setSM((CScfStateMachine*)p_cam_sm);

	iResourceSharerManager.setSM((CScfStateMachine*)p_cam_sm);

	COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
	p_cam_sm->ePreset = OMXCam->mRAWPreset.ePreset;
	p_cam_sm->extradataCircularBuffer_init();
	p_cam_sm->bTrace_latency = OMX_TRUE;
    OMXCam->mPictureSize.nOutputResWidth  = iSensor->getBayerWidth(); 
    OMXCam->mPictureSize.nOutputResHeight = iSensor->getBayerHeight(); 
	for(int i=0; i< TT_MAX ; i++)
	{
		p_cam_sm->iTimed_Task[i].current_time = 0;
		p_cam_sm->iTimed_Task[i].timeout = TT_TIMEOUT;
		p_cam_sm->iTimed_Task[i].timer_running = FALSE;
	}

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
    if (iSkipNVM)
    {
      i3AComponentManager.SetNVMData(OMXCam->mEepromdata.eepromData);
    }
#endif
}

/*
 * Destroy CAM_SM
 */
void Camera::destroySM(void){
    delete p_cam_sm;
    p_cam_sm = NULL;
}


/* */
/* Processing Component constructor
*/
Camera::Camera(ENS_Component &enscomp, t_uint8 manufacturer, t_uint16 model, t_uint8 revisionNumber, enumCameraSlot cam) :
    IFM_HostNmfProcessingComp(enscomp)
    ,iOpModeMgr(&enscomp)
    ,iTestMode()
    ,iCheckIntervalMsec(DEFAULT_CHECK_ALIVE_INTERVAL_MSEC)
    ,iPrintStatus(DEFAULT_CHECK_ALIVE_PRINT_STATUS)
    ,iCheckAliveTimerRunning(false)
    ,iNbAliveIterval500Msec(0)
    ,iNbLowPowerInterval500Msec(0)
    ,iCheckTimerRunning(false)
    ,iSharedMemory(&enscomp)
    ,iOmxStateMgr()
    ,iSiaPlatformManager(&enscomp)
    ,iIspctlComponentManager(SIA_CLIENT_CAMERA, enscomp)
    ,i3AComponentManager(&enscomp)
    ,iFramerate(camport::DEFAULT_VPB2_FRAMERATE * 100 / (1 << 16))  // Use VPB2 default framerate as backup value (to force fixed framerate mode when VPB2 is active).
    ,iIspctlCom(SIA_CLIENT_CAMERA, &enscomp)
    ,iGrabControl(enscomp, &iOpModeMgr)
    ,iGrab()
    ,iCapture_context()
    ,iZoom((COmxCamera*)&enscomp, &((COmxCamera*)&mENSComponent)->mSensorMode)
    ,iTrace()
    ,iResourceSharerManager(SIA_CLIENT_CAMERA, &enscomp)
    ,iDeferredEventMgr()
    ,iCallback(this)
    ,iTuningLoaderManager(&enscomp)
    ,iIspDampers(&enscomp)
    ,iPictureSettings(&enscomp)
    ,CapturePort(CAMERA_PORT_OUT1)
    ,RecordPort(CAMERA_PORT_OUT2)
    ,RawCapturePort(CAMERA_PORT_OUT1)
    ,iSelfTest(&enscomp,cam,&(this)->iDeferredEventMgr)
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
    ,iSkipNVM(OMX_FALSE)
#endif

{
	/* +CR337836 */
	//GET_AND_SET_TRACE_LEVEL(hsmcamera);
	/* -CR337836 */
	IN0("\n");
    OstTraceInt0(TRACE_FLOW, "Entry Camera::Camera");
    t_sensorDesciptor sensorDesc;
    // Initialisation to remove Coverity Warning
    sensorDesc.manufacturer = 0;
    sensorDesc.model        = 0;
    sensorDesc.revisionNumber = 0;
    sensorDesc.Fuse_Id[0] = 0;
    sensorDesc.Fuse_Id[1] = 0;
    sensorDesc.Fuse_Id[2] = 0;
    sensorDesc.Fuse_Id4   = 0;

    // Sensor related initializations
    switch(cam)
    {
      case ePrimaryCamera:
          iSensor = new CSensorPrimary();

          MSG2("XP70 firmware type \"%s\" version %s\n", Isp8500_primary_fw_split_type, Isp8500_primary_fw_split_version);
		  //OstTraceInt2(TRACE_DEBUG, "XP70 firmware type \"%s\" version %s", Isp8500_primary_fw_split_type, Isp8500_primary_fw_split_version);
          iIspctlComponentManager.setFWData(ISP8500_PRIMARY_FW_DATA_SIZE, Isp8500_primary_fw_data);
          iIspctlComponentManager.setFWSplit(ISP8500_PRIMARY_FW_SPLIT_SIZE, Isp8500_primary_fw_split);
          iIspctlComponentManager.setFWExt(ISP8500_PRIMARY_FW_EXT_SIZE, Isp8500_primary_fw_ext);
          iIspctlComponentManager.setFWExtDdr(ISP8500_PRIMARY_FW_EXT_DDR_SIZE, Isp8500_primary_fw_ext_ddr);
          sensorDesc.model = model;
          sensorDesc.manufacturer = manufacturer;
          sensorDesc.revisionNumber = revisionNumber;
          iSensor->SetSensorDescriptor(&sensorDesc);
	      iIspctlCom.setIspctlSensor(iSensor);	
          break;
      case eSecondaryCamera:
          iSensor = new CSensorSecondary();

          MSG2("XP70 firmware type \"%s\" version %s\n", Isp8500_secondary_fw_split_type, Isp8500_secondary_fw_split_version);
		  //OstTraceInt2(TRACE_DEBUG, "XP70 firmware type \"%s\" version %s", Isp8500_secondary_fw_split_type, Isp8500_secondary_fw_split_version);
          iIspctlComponentManager.setFWData(ISP8500_SECONDARY_FW_DATA_SIZE, Isp8500_secondary_fw_data);
          iIspctlComponentManager.setFWSplit(ISP8500_SECONDARY_FW_SPLIT_SIZE, Isp8500_secondary_fw_split);
          iIspctlComponentManager.setFWExt(ISP8500_SECONDARY_FW_EXT_SIZE, Isp8500_secondary_fw_ext);
          iIspctlComponentManager.setFWExtDdr(ISP8500_SECONDARY_FW_EXT_DDR_SIZE, Isp8500_secondary_fw_ext_ddr);
          sensorDesc.model = model;
          sensorDesc.manufacturer = manufacturer;
          sensorDesc.revisionNumber = revisionNumber;
          iSensor->SetSensorDescriptor(&sensorDesc);
	      iIspctlCom.setIspctlSensor(iSensor);	
          break;
      default:
         // A target device must have been set in CameraFactoryMethod().
         DBC_ASSERT(0);
    }
    //Default Lense parametrs
	mLensParameters.nSize = (OMX_U32)sizeof(OMX_STE_CONFIG_LENSPARAMETERTYPE);
	mLensParameters.nVersion.nVersion = OMX_VERSION_1_1_2;
    t_LensParameters  LensParameters;

    iSensor->GetLensParameters(&LensParameters);
    mLensParameters.nFocalLength = LensParameters.FocalLength;
    mLensParameters.nHorizontolViewAngle = LensParameters.HorizontolViewAngle;
    mLensParameters.nVerticalViewAngle = LensParameters.VerticalViewAngle;

    pTuningDataBase = NULL;

    iOpModeMgr.Init(RawCapturePort,RecordPort,
            (camport*)mENSComponent.getPort(CAMERA_PORT_OUT0),
            (camport*)mENSComponent.getPort(CAMERA_PORT_OUT1),
            (camport*)mENSComponent.getPort(CAMERA_PORT_OUT2));

    /* default configuration */

    OMX_PARAM_PORTDEFINITIONTYPE defaultParam;
    defaultParam.nPortIndex = CAMERA_PORT_OUT0;
    defaultParam.format.video.nFrameWidth = camport::DEFAULT_VPB0_WIDTH;
    defaultParam.format.video.nFrameHeight =camport::DEFAULT_VPB0_HEIGHT;
    defaultParam.format.video.nSliceHeight =camport::DEFAULT_VPB0_HEIGHT;
    defaultParam.format.video.eColorFormat=camport::DEFAULT_COLOR_FORMAT;
    defaultParam.format.video.xFramerate = camport::DEFAULT_VPB0_FRAMERATE; // The default framerate is port-specific. Use temporarily VPB0 for general initialization.
    camport* port = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT0);
    port->setDefaultFormatInPortDefinition(&defaultParam);

    defaultParam.nPortIndex = CAMERA_PORT_OUT2;
    defaultParam.format.video.eColorFormat = camport::DEFAULT_VPB2_COLOR_FORMAT;
    port = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT2);
    if(cam==ePrimaryCamera)
    {
        defaultParam.format.video.nFrameWidth = camport::DEFAULT_VPB2_WIDTH_PRIMARY;
        defaultParam.format.video.nFrameHeight =camport::DEFAULT_VPB2_HEIGHT_PRIMARY;
        defaultParam.format.video.nSliceHeight =camport::DEFAULT_VPB2_HEIGHT_PRIMARY;
    }
    else
    {
        defaultParam.format.video.nFrameWidth = camport::DEFAULT_VPB2_WIDTH_SECONDARY;
        defaultParam.format.video.nFrameHeight =camport::DEFAULT_VPB2_HEIGHT_SECONDARY;
        defaultParam.format.video.nSliceHeight =camport::DEFAULT_VPB2_HEIGHT_SECONDARY;
    }
    port->setDefaultFormatInPortDefinition(&defaultParam);

    defaultParam.nPortIndex = CAMERA_PORT_OUT1;
    defaultParam.format.video.nFrameWidth = iSensor->getBayerWidth();
    defaultParam.format.video.nFrameHeight = iSensor->getBayerHeight();
    defaultParam.format.video.nSliceHeight = defaultParam.format.video.nFrameHeight;
    defaultParam.format.video.eColorFormat = camport::DEFAULT_BAYER_COLOR_FORMAT;
    port = (camport*)mENSComponent.getPort(CAMERA_PORT_OUT1);
    port->setDefaultFormatInPortDefinition(&defaultParam);
  MSG0("Outside set Default Format port");
    latency_ShotToShotStart = 0;
    latency_ShotToSnapshotStart = 0;
    latency_TotalShutterLagStart = 0;
    latency_VFOffToOnStart = 0;
    latency_AFStart = 0;
	latency_ShotToVF = 0;
    iPanic = OMX_FALSE;

	/*For Shutterlag */
	Sw3aStopReqStatus = SW3A_STOP_NOT_REQUESTED;
	GrabAbortStatus = GRAB_ABORT_NOT_REQUESTED;

	iIspctlComponentManager.mEventClbks.setEventClbk(SIA_CLIENT_CAMERA, ISP_VALID_BMS_FRAME_NOTIFICATION, (Camera_ispctlInfo), (t_ispctlEventClbkCtxtHnd)(&enscomp));
	//performance traces start
	memset(&(mlatency_Hsmcamera),0,sizeof(mlatency_Hsmcamera));
	mlatency_Hsmcamera.nSize = sizeof(IFM_LATENCY_HSMCAMERA);
	mlatency_Hsmcamera.nVersion.nVersion = OMX_VERSION_1_1_2;
	//performance traces end
	iExecutingStateStartTime = 0;

	for (t_uint8 i=0; i < OMX_NUM_INDEXES_FOR_DAMPER_CONFIG; i++)
	{
	       isDamperConfigReqd[i].bIndexToBeConfigured = OMX_FALSE;
	}

    for (int i = 0; i < CAMERA_NB_OUT_PORTS; i++) {
        memset(&grabErrorDetail[i], 0, sizeof(GrabErrorDetail));
    }

	OUTR(" ",(0));
    OstTraceInt0(TRACE_FLOW, "Exit Camera::Camera");
}

/* */
/* setOperatingMode
*/
OMX_ERRORTYPE Camera::setOperatingMode(t_operating_mode_camera aOpMode) {
    iOpModeMgr.CurrentOperatingMode = aOpMode;
    return OMX_ErrorNone;
}

/* */
/* setExecutingStateStartTime
*/
void Camera::setExecutingStateStartTime() {
	iExecutingStateStartTime = (OMX_TICKS)omxilosalservices::OmxILOsalTimer::GetSystemTime();
}

OMX_TICKS Camera::getTimeElapsedSinceExecutingStateStart() {
	OMX_TICKS nTimeNow = (OMX_TICKS)omxilosalservices::OmxILOsalTimer::GetSystemTime();
	return nTimeNow - iExecutingStateStartTime;
}

/*
*/
void Camera::setPipes(e_grabPipeID pipe4port0, e_grabPipeID pipe4port1, e_grabPipeID pipe4port2) {

    MSG0("Camera setPipes\n");
    OstTraceInt1(TRACE_FLOW, "Entry Camera::setPipes:%p",(unsigned int)&mIgrabctlConfigure);
    grabPipeIDConfig[CAMERA_PORT_OUT0] = pipe4port0;
    grabPipeIDConfig[CAMERA_PORT_OUT0] = pipe4port1;
    grabPipeIDConfig[CAMERA_PORT_OUT0] = pipe4port2;
    mIgrabctlConfigure.setPort2PipeMapping(pipe4port0,pipe4port1,pipe4port2);
}
/* */
/* Processing Component destructor
*/
Camera::~Camera() {
    IN0("\n");
    MSG0("Camera deleted\n");
    OstTraceInt0(TRACE_FLOW, "Entry Camera::~Camera");
	OstTraceInt0 (TRACE_DEBUG, "Camera deleted");
    delete(iSensor); //Virtual destructor
    OUTR(" ",(0));
    OstTraceInt0(TRACE_FLOW, "Exit Camera::~Camera");
}

/* */
/* not used
*/
OMX_ERRORTYPE Camera::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    MSG0("Camera has no input port\n");
	OstTraceFiltInst0(TRACE_DEBUG, "Camera has no input port");
    return OMX_ErrorNone;
}

/* */
/* Camera::retrieveConfig */
/* */
OMX_ERRORTYPE Camera::retrieveConfig(OMX_INDEXTYPE aIdx, OMX_PTR p) {
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::retrieveConfig");
    OMX_ERRORTYPE err=OMX_ErrorNone;
    MSG1("Camera::retrieveConfig: param aIdx is 0x%x\n",aIdx);
    OstTraceFiltInst1(TRACE_DEBUG, "Camera::retrieveConfig: param aIdx is 0x%x",aIdx);
    switch ((OMX_U32)aIdx) {
        // 3A related indexes
        case OMX_IndexConfigFocusControl:
        case OMX_IndexConfigCommonWhiteBalance:
        case OMX_IndexConfigCommonBrightness:
        case OMX_IndexConfigCommonExposureValue:
        case OMX_IndexConfigCommonExposure:
        case OMX_IndexConfigCommonContrast:
        case OMX_IndexConfigCommonSaturation:
        case OMX_Symbian_IndexConfigMotionLevel:
        case OMX_Symbian_IndexConfigNDFilterControl:
        case OMX_Symbian_IndexConfigExposureLock:
        case OMX_Symbian_IndexConfigWhiteBalanceLock:
        case OMX_Symbian_IndexConfigFocusLock:
        case OMX_Symbian_IndexConfigAllLock:
        case OMX_Symbian_IndexConfigROI:
        case OMX_Symbian_IndexConfigSceneMode:
        case OMX_Symbian_IndexConfigSharpness:
        case OMX_Symbian_IndexConfigFlickerRemoval:
        case OMX_IndexConfigCommonImageFilter:
        case OMX_Symbian_IndexConfigFlashControl:
        case OMX_Symbian_IndexConfigVideoLight:
        case OMX_Symbian_IndexConfigFocusRange:
        case OMX_Symbian_IndexConfigFocusRegion:
        case OMX_Symbian_IndexConfigExtFocusStatus:
        case OMX_Symbian_IndexConfigBracketing:
	    case OMX_Symbian_IndexConfigRGBHistogram:		
        case IFM_IndexConfig3ATraceDumpControl:
        case OMX_Symbian_IndexConfigPreCaptureExposureTime:
        case OMX_Symbian_IndexConfigAnalyzerFeedback:
        case OMX_STE_IndexConfigMeteringOn:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
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
        case OMX_Symbian_IndexConfigExtDigitalZoom:
        {
            OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE *pZoomConfig = (OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)p;
            pZoomConfig->xZoomFactor = iZoom.pZoomFactorStatus->xZoomFactor;
            break;
        }
        case OMX_Symbian_IndexConfigCenterFieldOfView:
        {
            OMX_SYMBIAN_CONFIG_POINTTYPE *pCenterFOV = (OMX_SYMBIAN_CONFIG_POINTTYPE*)p;
            pCenterFOV->sPoint.nX = iZoom.pCenterFOVStatus->sPoint.nX;
            pCenterFOV->sPoint.nY = iZoom.pCenterFOVStatus->sPoint.nY;
            break;
        }
        case IFM_IndexConfigCommonWritePe :
        {
            IFM_CONFIG_WRITEPETYPE * pPE = (IFM_CONFIG_WRITEPETYPE *)p;
            *pPE = iTrace.iListOfPeToWrite;
            break;
        }
        case IFM_IndexConfigCommonReadPe :
        {
            IFM_CONFIG_READPETYPE * pPE = (IFM_CONFIG_READPETYPE *)p;
            *pPE = iTrace.iListOfPeToRead;
            break;
        }
        case IFM_IndexConfigCommonOpMode_Check:
        {
            IFM_CONFIG_OPMODE_CHECK * pOpMode = (IFM_CONFIG_OPMODE_CHECK *)p;
            pOpMode->eOpMode = iOpModeMgr.CurrentOperatingMode;
            break;
        }
        case IFM_IndexConfigCheckAlive:
        {
            IFM_CONFIG_CHECK_ALIVE *pCheckAlive = (IFM_CONFIG_CHECK_ALIVE *)p;
            pCheckAlive->nCheckIntervalMsec = (OMX_U32)iCheckIntervalMsec;
            pCheckAlive->bPrintStatus = iPrintStatus ? OMX_TRUE : OMX_FALSE;
            break;
        }
        case OMX_IndexConfigCallbackRequest:
        {
            OMX_CONFIG_CALLBACKREQUESTTYPE* pCallbackRequest = (OMX_CONFIG_CALLBACKREQUESTTYPE*)p;
            switch((t_uint32)pCallbackRequest->nIndex) {
                case OMX_Symbian_IndexConfigExposureLock:
                case OMX_Symbian_IndexConfigWhiteBalanceLock:
                case OMX_Symbian_IndexConfigFocusLock:
                case OMX_Symbian_IndexConfigAllLock:
                case OMX_Symbian_IndexConfigPreCaptureExposureTime:
                case OMX_Symbian_IndexConfigExtFocusStatus:
                case OMX_STE_IndexConfigMeteringOn:
                    pCallbackRequest->bEnable = OMX_FALSE; // TODO: need a getter at sw3A level.
                    break;
                case OMX_Symbian_IndexConfigExposureInitiated:
                    break;
                case OMX_Symbian_IndexConfigCameraTesting:
                    if (OMX_ALL == pCallbackRequest->nPortIndex)
                    {
                        pCallbackRequest->bEnable = iSelfTest.callbackEnabled;
                    }
                    else
                    {
                        err = OMX_ErrorUnsupportedSetting;
                    }
                    break;
                case OMX_STE_IndexConfigCommonZoomMode:
                    {
                        COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
                        pCallbackRequest->bEnable = OMXCam->mZoomModeClbkReq.bEnable;
                    }
                    break;
                default:
                    err = OMX_ErrorUnsupportedSetting;
            }
            break;
        }
        case OMX_Symbian_IndexConfigOrientationScene:
            *(OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE*)p = ((COmxCamera*)&mENSComponent)->mSceneOrientation;
            break;
        case OMX_Symbian_IndexConfigCameraSensorInfo:
        {
            OMX_SYMBIAN_CONFIG_HWINFOTYPE * pSensorInfoType = (OMX_SYMBIAN_CONFIG_HWINFOTYPE*)p;
            MSG0("Parsing HWINFO data from sensor data structure\n");
            OstTraceFiltInst0(TRACE_DEBUG, "Parsing HWINFO data from sensor data structure");
            pSensorInfoType->nVersion1 =   (iSensor->GetSensorManufacturer()&0xff)
                                         |((iSensor->GetSensorRevNumber()&0xff)<<8)
                                         |((iSensor->GetSensorModel()&0xffff)<<16);
            pSensorInfoType->nVersion2 =   (iSensor->GetFuseId4 ()&0xFFFFFF);
            if (pSensorInfoType->cInfoString) {
                const char * pConfigFileName = NULL;
                if (iTuningLoaderManager.getCameraConfigFileName(&pConfigFileName) == TUNING_LOADER_MGR_OK){
                    strncpy(pSensorInfoType->cInfoString,pConfigFileName,127);
                }
            }
            break;
        }
        case OMX_Symbian_IndexConfigFlashGunInfo:
        {
            OMX_SYMBIAN_CONFIG_HWINFOTYPE * pFlashGunInfoType = (OMX_SYMBIAN_CONFIG_HWINFOTYPE*)p;
            pFlashGunInfoType->nVersion1 = 0x00000101; // Info B, info A, Flash IC revision, Flash IC info
            pFlashGunInfoType->nVersion2 = 0;
#if 0
            MSG0("Workaround for testing despite MMTE poor OMX_STRING support\n");
            OstTraceFiltInst0(TRACE_DEBUG, "Workaround for testing despite MMTE poor OMX_STRING support");
            if (!pFlashGunInfoType->cInfoString)
                pFlashGunInfoType->cInfoString = new char[128];
#endif
            if (pFlashGunInfoType->cInfoString) {
                const char * pConfigFileName;
                if (TUNING_LOADER_MGR_OK == iTuningLoaderManager.getFlashConfigFileName(&pConfigFileName)){
                    strncpy(pFlashGunInfoType->cInfoString,pConfigFileName,127);
                }
                else err = OMX_ErrorNotReady;
            }
            break;
        }
        case OMX_Symbian_IndexConfigSupportedResolutions:
        {
            OMX_SYMBIAN_CONFIG_SUPPORTEDRESOLUTIONS * pRes = (OMX_SYMBIAN_CONFIG_SUPPORTEDRESOLUTIONS*)p;
            err = getSupportedResolutions(pRes);
            break;
        }
        case OMX_Symbian_IndexConfigSensorNvmData:
        {
            OMX_SYMBIAN_CONFIG_SENSORNVMDATATYPE* pSensorNvmData = (OMX_SYMBIAN_CONFIG_SENSORNVMDATATYPE*)p;
            if(pSensorNvmData->nNVMBytes == 0)
            {
                // IL Client is requesting the sensor NVM byte size
                pSensorNvmData->nNVMBytes = (OMX_U32)pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA)->getSize();
            }
            else
            {
                // Copy the sensor NVM payload into IL client buffer
                if (NULL != pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA)->getAddr()) {
                    memcpy( pSensorNvmData->NVMData,
                        pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA)->getAddr(),
                        MIN(pSensorNvmData->nNVMBytes, (OMX_U32)pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA)->getSize()));
                }
                else {
                    err = OMX_ErrorUndefined;
                }
            }
            break;
        }
		
	 case OMX_STE_IndexConfigCommonZoomMode:
	 	MSG1("retrieveConfig. [smooth zoom] SmoothZoom = %s\n", (((COmxCamera*)&mENSComponent)->mZoomMode.bEnabled == OMX_TRUE)?"ENABLED":"DISABLED");
	 	*(OMX_CONFIG_BOOLEANTYPE*)p = ((COmxCamera*)&mENSComponent)->mZoomMode;
	 	break;

	case OMX_STE_IndexConfigCommonLensParameters:
    {
        /*Focallength will be updated by sw3A as it changes dynamically*/
        *(OMX_STE_CONFIG_LENSPARAMETERTYPE*)p = mLensParameters;
        break;
    }
	case OMX_STE_IndexConfigPictureSize:
	{
	  *(OMX_STE_CONFIG_PICTURESIZETYPE*)p = ((COmxCamera*)&mENSComponent)->mPictureSize;
	      break;
	}
#ifdef FREQ_SCALING_ALLOWED
    case OMX_STE_IndexConfigCommonScaling:
    {
        COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
        (*(IFM_CONFIG_SCALING_MODETYPE *)p)= OMXCam->mfreqscaling;
        break;
    }
#endif //FREQ_SCALING_ALLOWED
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
        case OMX_3A_IndexConfigIspParameters:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
        case OMX_3A_IndexConfigExposure:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
        case OMX_3A_IndexConfigFocus:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
        case OMX_3A_IndexConfigFlashSynchro:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
        case OMX_3A_IndexConfigMaxFramerate:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
        case OMX_3A_IndexConfigSensorMode:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
        case OMX_3A_IndexConfigEEPROMData:
            err = i3AComponentManager.getConfig(aIdx, p);
            break;
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
        case OMX_STE_IndexConfigExifTimestamp:
            {
                COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
                (*(OMX_TIME_CONFIG_TIMESTAMPTYPE*)p)= OMXCam->mExifTimeStamp;
                break;
            }
        default:
            err = mENSComponent.getConfig(aIdx,p);
            break;
    }
    OUTR(" ",(err));
    OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::retrieveConfig (%d)", err);
    return err;
}


OMX_ERRORTYPE Camera::getSupportedResolutions(OMX_SYMBIAN_CONFIG_SUPPORTEDRESOLUTIONS *pRes) {
  OMX_ERRORTYPE err = OMX_ErrorNone;
  // retrieve list from sensor
  t_sensorResolutions res;
  MSG0("trying to retrieve resolutions from sensor\n");
  OstTraceFiltInst0(TRACE_DEBUG, "trying to retrieve resolutions from sensor");
  if (iSensor->getResolutions(&res)) {
    MSG1("Get resolutions count=%d\n", res.count);
    OstTraceFiltInst1(TRACE_DEBUG, "Get resolutions count=%d", res.count);
    /* filter by aspect ratio if set */
    int filter = 0;
    if (pRes->eAspectRatio == OMX_SYMBIAN_AspectRatioUnspecified)
      filter = ASPECT_RATIO_ALL;
    else if (pRes->eAspectRatio == OMX_SYMBIAN_AspectRatio4To3)
      filter = ASPECT_RATIO_4TO3;
    else if (pRes->eAspectRatio == OMX_SYMBIAN_AspectRatio16To9)
      filter = ASPECT_RATIO_16TO9;
    else if (pRes->eAspectRatio == OMX_SYMBIAN_AspectRatio3To2)
      filter = ASPECT_RATIO_3TO2;
    else if (pRes->eAspectRatio == OMX_SYMBIAN_AspectRatio1To1)
      filter = ASPECT_RATIO_1TO1;
    else if (pRes->eAspectRatio == OMX_SYMBIAN_AspectRatioNonStandard)
      filter = ASPECT_RATIO_OTHER;
    else
      filter = 0; // no resolutions shall be returned
    CSensor::filterResolutionsAspectRatio(&res, filter);

    if (pRes->nNumResolutions == 0) {
      /* user wanted to retrieve number of resolutions */
      MSG1("Returning number of resolutions count=%d\n", res.count);
      OstTraceFiltInst1(TRACE_DEBUG, "Returning number of resolutions count=%d", res.count);
      pRes->nNumResolutions = res.count;
    } else {
      /* user wanted to retrieve x-number of resolutions */
      MSG2("User requested %d resolutions count=%d\n", (int)pRes->nNumResolutions, (int)res.count);
      OstTraceFiltInst2(TRACE_DEBUG, "User requested %d resolutions count=%d", (int)pRes->nNumResolutions, (int)res.count);
      if (pRes->nNumResolutions > (unsigned int)res.count)
      pRes->nNumResolutions = res.count;

      bool res_listed[MAX_SENSOR_RESOLUTIONS];
      for (int i = 0; i < MAX_SENSOR_RESOLUTIONS; i++) {
        res_listed[i] = false;
      }

      for (unsigned int i = 0; i < pRes->nNumResolutions; i++) {
        OMX_SYMBIAN_SIZETYPE *c = &pRes->sResolutions[i];
        // resolutions need to be in decreasing size order, so find
        // biggest non-listed resolution
        t_sensorResolution *big = NULL;
        int big_idx = -1;
        for (int j = 0; j < res.count; j++) {
          if (!res_listed[j] && (big_idx == -1 || (res.res[big_idx].x < res.res[j].x
                           && res.res[big_idx].y < res.res[j].y)))
            big_idx = j;
        }
        DBC_ASSERT(big_idx >= 0 && big_idx < MAX_SENSOR_RESOLUTIONS);
        if (big_idx < 0)
          big_idx = 0;
        if (big_idx >= MAX_SENSOR_RESOLUTIONS)
        big_idx = 0;
        big = &res.res[big_idx];
        res_listed[big_idx] = true;
        DBC_ASSERT(big);
        if (big) {
          c->nWidth = big->x;
          c->nHeight = big->y;
          // add number of non-active lines to y
          c->nHeight += iSensor->getNberOfNonActiveLines();
        } else {
          c->nWidth = 0;
          c->nHeight = 0;
        }
      }
    }
  } else err = OMX_ErrorUnsupportedSetting;
  return err;
}



/* */
/* Camera::applyConfig */
/* */
OMX_ERRORTYPE Camera::applyConfig( OMX_INDEXTYPE nIndex, OMX_PTR pStructure)
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	IN0("\n");
    OstTraceInt0 (TRACE_FLOW, "Entry Camera::applyConfig");
	s_scf_event event;
	if (pStructure == 0) {
		OUTR(" ",OMX_ErrorBadParameter);
		OstTraceInt0(TRACE_FLOW, "Exit Camera::applyConfig OMX_ErrorBadParameter");
		return OMX_ErrorBadParameter;
	}
	COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
	switch ((t_uint32)nIndex) {
		case OMX_Symbian_IndexConfigCommonExtCapturing:
		{
		    OMX_SYMBIAN_CONFIG_BOOLEANTYPE *corresponding_omxcamera_config;
            switch(((OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)pStructure)->nPortIndex)
            {
                case CAMERA_PORT_OUT1:
                    corresponding_omxcamera_config = &OMXCam->mCapturingStruct_VPB1;
                    break;
                case CAMERA_PORT_OUT2:
                    corresponding_omxcamera_config = &OMXCam->mCapturingStruct_VPB2;
                    break;
                default:
                    return OMX_ErrorBadParameter;
            }
            OMX_STATETYPE currentState = OMX_StateInvalid;
            mENSComponent.GetState(&mENSComponent, &currentState);
			MSG0("Capturing bit received");
            if((currentState == OMX_StateExecuting) || (currentState == OMX_StatePause)) {
                if (CAMERA_PORT_OUT1 == ((OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)pStructure)->nPortIndex)
                {
                	//performance traces start
                	p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
                	mlatency_Hsmcamera.e_OMXCAM_ShotToSnapshotStart.t1 = p_cam_sm->mTime;
                	mlatency_Hsmcamera.e_OMXCAM_ShotToSave_Start.t1 = p_cam_sm->mTime;
                	latency_ShotToSnapshotStart = 1;
					OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToSnapshot %d", 1);
					OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToSave %d", 1);
                    if (0 == latency_ShotToShotStart) {
                    	p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
                    	mlatency_Hsmcamera.e_OMXCAM_ShotToShot.t1 = p_cam_sm->mTime;
                    	latency_ShotToShotStart = 1;
                    	OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToShot %d", 1);
                    }
                    //performance traces end
                }

				if (CAMERA_PORT_OUT1 == ((OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)pStructure)->nPortIndex &&  iOpModeMgr.ConfigCapturing[CAMERA_PORT_OUT2] == true)					//Issue here
				{
					PERF_TRACE_LAT("Start Capture Request",p_cam_sm->tempo1);					
					event.type.other=0;
					event.sig = START_HIDDEN_BMS_SIG;
					p_cam_sm->GrabStatusNonDefferred=1;
			        event.type.capturingStruct = *corresponding_omxcamera_config;
		            iDeferredEventMgr.queueNewEvent(&event);
					s_scf_event devent;
		            devent.sig = Q_DEFERRED_EVENT_SIG;
		            p_cam_sm->ProcessEvent(&devent);
				}
				else
				{
                event.type.other=0;
                event.sig = CAMERA_EXIT_PREVIEW_SIG;
                event.type.capturingStruct = *corresponding_omxcamera_config;
                iDeferredEventMgr.queueNewEvent(&event);

                s_scf_event devent;
                devent.sig = Q_DEFERRED_EVENT_SIG;
                p_cam_sm->ProcessEvent(&devent);
				}
            } else {
                t_uint16 portIndex=corresponding_omxcamera_config->nPortIndex;
                iOpModeMgr.ConfigCapturing[portIndex] = corresponding_omxcamera_config->bEnabled;
                MSG1("OMX_IndexConfigCapturing set to %d in Loaded or Idle state, will be applied on executing state callback\n", iOpModeMgr.ConfigCapturing[portIndex]);
		        OstTraceInt1 (TRACE_DEBUG, "OMX_IndexConfigCapturing set to %d in Loaded or Idle state, will be applied on executing state callback", iOpModeMgr.ConfigCapturing[portIndex]);
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
            if (OMX_TRUE == iOpModeMgr.ConfigCapturing[RawCapturePort])
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
            OMXCam->mCameraUserSettings.bContinuous = OMXCam->mCaptureModeType.bContinuous;
            OMXCam->mCameraUserSettings.bFrameLimited = OMXCam->mCaptureModeType.bFrameLimited;
            OMXCam->mCameraUserSettings.nFrameLimit = OMXCam->mCaptureModeType.nFrameLimit;
            break;
        }
        case OMX_Symbian_IndexConfigExtCaptureMode:
        {
            OMX_STATETYPE currentState = OMX_StateInvalid;
            mENSComponent.GetState(&mENSComponent, &currentState);

            if (OMX_TRUE == iOpModeMgr.ConfigCapturing[RawCapturePort])
            {
                return OMX_ErrorNotReady;
            }
            /* if still VF is running then start BMS capture now */
            else if (((OpMode_Cam_StillPreview == iOpModeMgr.CurrentOperatingMode)
                    || (OpMode_Cam_StillFaceTracking == iOpModeMgr.CurrentOperatingMode))
                && ((OMX_StateExecuting == currentState)
                    || (OMX_StatePause == currentState)))
            {
                event.type.other=0;
                event.sig = CAMERA_INDEX_CONFIG_EXTCAPTUREMODE_SIG;
                event.type.extCapturingStruct = OMXCam->mExtCaptureModeType;
                iDeferredEventMgr.queueNewEvent(&event);

                s_scf_event devent;
                devent.sig = Q_DEFERRED_EVENT_SIG;
                p_cam_sm->ProcessEvent(&devent);
            }
            /* else only store config */
            /* we will start BMS capture when starting next still VF */
            else
            {
                iOpModeMgr.SetFrameBefore(OMXCam->mExtCaptureModeType.nFrameBefore);
                iOpModeMgr.SetPrepareCapture(OMXCam->mExtCaptureModeType.bPrepareCapture);
            }
            OMXCam->mCameraUserSettings.nFramesBefore = OMXCam->mExtCaptureModeType.nFrameBefore;
            break;
        }
        case OMX_IndexConfigCommonColorBlend:
            // Not implemented
            break;
	case OMX_Symbian_IndexConfigRGBHistogram:
	    // Not implemented
            break;
        case OMX_Symbian_IndexConfigExtDigitalZoom :
        {
	     if (OMXCam->mZoomMode.bEnabled == OMX_TRUE 
		 	&& p_cam_sm->pZoom->ZoomFactor.xZoomFactor.nValue == ((OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)pStructure)->xZoomFactor.nValue)
	     	{
	     		//do nothing
//	     		MSG1("applyConfig. [smooth zoom] Current zoom value equal to requested value. No Zoom Done\n");
	     		break;
	     	}

//		 MSG1("applyConfig. [smooth zoom] Current zoom nValue = %d, requested nValue = %d\n", p_cam_sm->pZoom->ZoomFactor.xZoomFactor.nValue, 
//		 	((OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)pStructure)->xZoomFactor.nValue);

            OMX_STATETYPE currentState = OMX_StateInvalid;
            mENSComponent.GetState(&mENSComponent, &currentState);
            if((currentState == OMX_StateExecuting) || (currentState == OMX_StatePause)) {
                event.type.other=0;
                event.sig = CAMERA_INDEX_CONFIG_ZOOM_SIG;
                event.args.omxsetConfigIndex = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtDigitalZoom;
                t_bool bNewEventQueued;
                iDeferredEventMgr.queueZoomEvent(&event, &bNewEventQueued);
                if(bNewEventQueued == true) {
                    s_scf_event devent;
                    devent.sig = Q_DEFERRED_EVENT_SIG;
                    p_cam_sm->ProcessEvent(&devent);
                }
            }
            OMXCam->mCameraUserSettings.xDigitalZoomFactor = ((OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)pStructure)->xZoomFactor;
            MSG3("Zoom: mCameraUserSettings.xDigitalZoomFactor nValue = %lu, nMin = %lu, nMax = %lu\n",
                        OMXCam->mCameraUserSettings.xDigitalZoomFactor.nValue, OMXCam->mCameraUserSettings.xDigitalZoomFactor.nMin, OMXCam->mCameraUserSettings.xDigitalZoomFactor.nMax);
            break;
        }
        case OMX_Symbian_IndexConfigCenterFieldOfView :
        {
            OMX_STATETYPE currentState = OMX_StateInvalid;
            mENSComponent.GetState(&mENSComponent, &currentState);
            if((currentState == OMX_StateExecuting) || (currentState == OMX_StatePause)) {
                event.type.other=0;
                event.sig = CAMERA_INDEX_CONFIG_ZOOM_SIG;
                event.args.omxsetConfigIndex = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCenterFieldOfView;
                t_bool bNewEventQueued;
                iDeferredEventMgr.queueZoomEvent(&event, &bNewEventQueued);
                if(bNewEventQueued == true) {
                    s_scf_event devent;
                    devent.sig = Q_DEFERRED_EVENT_SIG;
                    p_cam_sm->ProcessEvent(&devent);
                }
            }
            OMXCam->mCameraUserSettings.nX = ((OMX_SYMBIAN_CONFIG_POINTTYPE*)pStructure)->sPoint.nX;
            OMXCam->mCameraUserSettings.nY = ((OMX_SYMBIAN_CONFIG_POINTTYPE*)pStructure)->sPoint.nY;
            MSG2("Zoom: mCameraUserSettings nX = %lu, nY = %lu\n", OMXCam->mCameraUserSettings.nX, OMXCam->mCameraUserSettings.nY);
            break;
        }
        case OMX_Symbian_IndexConfigExtOpticalZoom :
            OMXCam->mCameraUserSettings.xOpticalZoomFactor = OMXCam->mOpticalZoomFactor.xZoomFactor;
            break;
        case OMX_Symbian_IndexConfigAFAssistantLight:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFlashAFAssist);
            break;
        case OMX_IndexConfigCommonWhiteBalance :
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mWhiteBalControl);
            OMXCam->mCameraUserSettings.eWhiteBalControl = OMXCam->mWhiteBalControl.eWhiteBalControl;
            configureDamper(OMX_IndexConfigCommonWhiteBalance);
            addIndexToDamperConfigList(OMX_IndexConfigCommonWhiteBalance);
            break;
        case OMX_IndexConfigCommonBrightness :
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mBrightness);
            OMXCam->mCameraUserSettings.nBrightness = OMXCam->mBrightness.nBrightness;
            break;
        case OMX_IndexConfigCommonExposureValue:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mExposureValue);
            OMXCam->mCameraUserSettings.eMetering         = OMXCam->mExposureValue.eMetering;
            OMXCam->mCameraUserSettings.xEVCompensation   = OMXCam->mExposureValue.xEVCompensation;
            OMXCam->mCameraUserSettings.nApertureFNumber  = OMXCam->mExposureValue.nApertureFNumber;
            OMXCam->mCameraUserSettings.bAutoAperture     = OMXCam->mExposureValue.bAutoAperture;
            OMXCam->mCameraUserSettings.nShutterSpeedMsec = OMXCam->mExposureValue.nShutterSpeedMsec;
            OMXCam->mCameraUserSettings.bAutoShutterSpeed = OMXCam->mExposureValue.bAutoShutterSpeed;
            OMXCam->mCameraUserSettings.nSensitivity      = OMXCam->mExposureValue.nSensitivity;
            OMXCam->mCameraUserSettings.bAutoSensitivity  = OMXCam->mExposureValue.bAutoSensitivity;
            configureDamper(OMX_IndexConfigCommonExposureValue);
            addIndexToDamperConfigList(OMX_IndexConfigCommonExposureValue);
	     break;
        case OMX_IndexConfigCommonExposure:
              err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mExposureControl);
              OMXCam->mCameraUserSettings.eExposureControl = OMXCam->mExposureControl.eExposureControl;
              configureDamper(OMX_IndexConfigCommonExposure);
              addIndexToDamperConfigList(OMX_IndexConfigCommonExposure);
              break;
        case OMX_IndexConfigCommonContrast:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mContrast);
            OMXCam->mCameraUserSettings.nContrast = OMXCam->mContrast.nContrast;
            break;
        case OMX_IndexConfigCommonSaturation:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mSaturation);
            OMXCam->mCameraUserSettings.nSaturation = OMXCam->mSaturation.nSaturation;
            break;
        case OMX_IndexConfigCommonImageFilter:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mImageFilterType);
            OMXCam->mCameraUserSettings.eImageFilter  = OMXCam->mImageFilterType.eImageFilter;
            configureDamper(OMX_IndexConfigCommonImageFilter);
            addIndexToDamperConfigList(OMX_IndexConfigCommonImageFilter);
            break;
        case OMX_IndexConfigFocusControl:
			//performance traces start
			p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
			mlatency_Hsmcamera.e_OMXCAM_ShotToShot.t1 = p_cam_sm->mTime;
			mlatency_Hsmcamera.e_OMXCAM_AF.t1 = p_cam_sm->mTime;
			latency_ShotToShotStart = 1;
		    latency_AFStart = 1;
			OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToShot %d", 1);
			OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_AF %d", 1);

		    if((OMX_IMAGE_FocusControlOn == OMXCam->mFocusControl.eFocusControl) || (OMX_IMAGE_FocusControlAuto == OMXCam->mFocusControl.eFocusControl))
		    {
		    	p_cam_sm->latencyMeasure(&(p_cam_sm->mTime));
		    	mlatency_Hsmcamera.e_OMXCAM_TotalShutterLag.t1 = p_cam_sm->mTime;
		    	latency_TotalShutterLagStart = 1;
		    	OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_TotalShutterLag %d", 1);

		    }
		    //performance traces end
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFocusControl);
            OMXCam->mCameraUserSettings.eFocusControl = OMXCam->mFocusControl.eFocusControl;
            OMXCam->mCameraUserSettings.nFocusSteps = OMXCam->mFocusControl.nFocusSteps;
            OMXCam->mCameraUserSettings.nFocusStepIndex = OMXCam->mFocusControl.nFocusStepIndex;
            break;
        case OMX_Symbian_IndexConfigFocusRegion:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFocusRegion);
            OMXCam->mCameraUserSettings.eFocusRegionControl = OMXCam->mFocusRegion.eFocusRegionControl;
            OMXCam->mCameraUserSettings.sFocusRegion = OMXCam->mFocusRegion.sFocusRegion;
            configureDamper((OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRegion);
            addIndexToDamperConfigList((OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRegion);
            break;
        case OMX_Symbian_IndexConfigFocusRange:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFocusRange);
            configureDamper((OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRange);
            addIndexToDamperConfigList((OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRange);
            break;
        case OMX_IndexConfigCommonFrameStabilisation:
        {
            /* Todo: check nPortIndex */
            iGrabControl.mStabEnabled = ((OMX_CONFIG_FRAMESTABTYPE*)pStructure)->bStab;
            OMXCam->mCameraUserSettings.bStab = ((OMX_CONFIG_FRAMESTABTYPE*)pStructure)->bStab;
            break;
        }
		case OMX_Symbian_IndexConfigMotionLevel:
		    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mMotionLevel);
		    i3AComponentManager.setConfig((OMX_INDEXTYPE)OMX_Symbian_IndexConfigMotionLevel, (OMX_PTR) &OMXCam->mMotionLevel);
		    break;
		case OMX_Symbian_IndexConfigNDFilterControl:
		    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mNDFilterControl);
		    break;
		case OMX_Symbian_IndexConfigExposureLock:
		{
		    OMX_SYMBIAN_CONFIG_LOCKTYPE* pExposureLock = &OMXCam->mExposureLock;
		    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR)pExposureLock);
		    OMXCam->mCameraUserSettings.nLockingStatus |= (pExposureLock->eImageLock != OMX_SYMBIAN_LockOff) << 0;
		    break;
      }
		case OMX_Symbian_IndexConfigWhiteBalanceLock:
		{
		    OMX_SYMBIAN_CONFIG_LOCKTYPE* pWhiteBalanceLock = &OMXCam->mWhiteBalanceLock;
		    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR)pWhiteBalanceLock);
		    OMXCam->mCameraUserSettings.nLockingStatus |= (pWhiteBalanceLock->eImageLock != OMX_SYMBIAN_LockOff) << 1;
		    break;
		}
		case OMX_Symbian_IndexConfigFocusLock:
		{
		    OMX_SYMBIAN_CONFIG_LOCKTYPE* pFocusLock = &OMXCam->mFocusLock;
		    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR)pFocusLock);
		    OMXCam->mCameraUserSettings.nLockingStatus |= (pFocusLock->eImageLock != OMX_SYMBIAN_LockOff) << 2;
		    break;
		}
        case OMX_Symbian_IndexConfigAllLock:
        {
            OMX_SYMBIAN_CONFIG_LOCKTYPE* pAllLock = &OMXCam->mAllLock;
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR)pAllLock);
            OMXCam->mCameraUserSettings.nLockingStatus = (pAllLock->eImageLock == OMX_SYMBIAN_LockOff ? 0x0 : 0x7);
            break;
        }
        case OMX_Symbian_IndexConfigROI:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mROI);
            break;
        case OMX_Symbian_IndexConfigFlashControl:
            if (ePrimaryCamera == iSensor->GetCameraSlot())
            {
                err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFlashControlStruct);
            }
            OMXCam->mCameraUserSettings.eFlashControl = OMXCam->mFlashControlStruct.eFlashControl;
            break;
        case OMX_Symbian_IndexConfigVideoLight:
            if (ePrimaryCamera == iSensor->GetCameraSlot())
		    {
		        err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mVideoLight);
		    }
		    // OMXCam->mCameraUserSettings.mVideoLight = OMXCam->mVideoLight.eControl; Todo: check whether this config should be part of UserSettings extradata.
		    break;
        case IFM_IndexConfigCommonReadPe :
        {
            event.type.other=0;
            event.type.readStruct = OMXCam->mIfmReadPEType;
            event.sig = SHARED_READLISTPE_DEBUG_SIG;
            iDeferredEventMgr.queueNewEvent(&event);
            s_scf_event devent;
            devent.sig = Q_DEFERRED_EVENT_SIG;
            p_cam_sm->ProcessEvent(&devent);
            break;
        }

        case IFM_IndexConfigCommonWritePe :
        {
            event.type.other=0;
            event.type.writeStruct = OMXCam->mIfmWritePEType;
            event.sig = SHARED_WRITELISTPE_SIG;
            iDeferredEventMgr.queueNewEvent(&event);
            s_scf_event devent;
            devent.sig = Q_DEFERRED_EVENT_SIG;
            p_cam_sm->ProcessEvent(&devent);
            break;
        }
        case IFM_IndexConfigCheckAlive :
        {
            iPrintStatus = OMXCam->mIfmCheckAlive.bPrintStatus == OMX_TRUE ? true : false;
            iCheckIntervalMsec = (unsigned int)OMXCam->mIfmCheckAlive.nCheckIntervalMsec;
            checkAliveStartStop();
            break;
        }
        case IFM_IndexConfigTestMode :
        {
            event.type.other=0;
            event.sig = CAMERA_INDEX_CONFIG_TEST_MODE_SIG;
            event.type.testModeStruct = OMXCam->mTestModeType;
            iDeferredEventMgr.queueNewEvent(&event);

            s_scf_event devent;
            devent.sig = Q_DEFERRED_EVENT_SIG;
            p_cam_sm->ProcessEvent(&devent);

            break;
        }
        case OMX_IndexConfigCallbackRequest :
        {
            switch((t_uint32)((OMX_CONFIG_CALLBACKREQUESTTYPE*)pStructure)->nIndex) {
                case OMX_Symbian_IndexConfigCameraTesting:
                    if (OMX_ALL == OMXCam->mCallbackRequests.CameraTesting.nPortIndex) {
                        iSelfTest.callbackEnabled = OMXCam->mCallbackRequests.CameraTesting.bEnable;
                        err = OMX_ErrorNone;
                    }
                    else { err = OMX_ErrorUnsupportedSetting; }
                    break;
                case OMX_Symbian_IndexConfigExposureLock:
                    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mCallbackRequests.ExposureLock);
                    break;
                case OMX_Symbian_IndexConfigWhiteBalanceLock:
                    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mCallbackRequests.WhiteBalanceLock);
                    break;
                case OMX_Symbian_IndexConfigFocusLock:
                    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mCallbackRequests.FocusLock);
                    break;
                case OMX_Symbian_IndexConfigAllLock:
                    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mCallbackRequests.AllLock);
                    break;
                case OMX_Symbian_IndexConfigExtFocusStatus:
                    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mCallbackRequests.ExtFocusStatus);
                    break;
                case OMX_Symbian_IndexConfigPreCaptureExposureTime:
                    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mCallbackRequests.PreCaptureExposureTime);
                    break;
                case OMX_Symbian_IndexConfigExposureInitiated:
                    break;
                case OMX_STE_IndexConfigCommonZoomMode:
                    break;
                case OMX_STE_IndexConfigMeteringOn:
                    err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mCallbackRequests.MeteringPrefashCallback);
                    break;
                default:
                    err = OMX_ErrorUnsupportedSetting;
            }
            break;
        }
        case IFM_IndexConfig3ATraceDumpControl:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mIfm3ATraceDumpControl);
            break;
        case OMX_Symbian_IndexConfigRAWPreset:
            p_cam_sm->ePreset = ((OMX_SYMBIAN_CONFIG_RAWPRESETTYPE*)pStructure)->ePreset;
            break;
        case OMX_IndexConfigCommonMirror:
        {
            p_cam_sm->eMirror = ((OMX_CONFIG_MIRRORTYPE*)pStructure)->eMirror;
            event.type.other=0;
            event.sig = CAMERA_INDEX_CONFIG_MIRROR_SIG;
            iDeferredEventMgr.queueNewEvent(&event);
            s_scf_event devent;
            devent.sig = Q_DEFERRED_EVENT_SIG;
            p_cam_sm->ProcessEvent(&devent);
            break;
        }
        case OMX_IndexConfigCommonRotate:
          /*Nothing to do here, all done in omxcamera.cpp*/
          break;
        case OMX_Symbian_IndexParamRevertAspectRatio:
          /*Nothing to do here, all done in omxcamera.cpp*/
          break;
         case OMX_Symbian_IndexConfigSceneMode:
        {
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mSceneMode);
            OMXCam->mCameraUserSettings.nSceneModePreset = OMXCam->mSceneMode.eSceneType;
            configureDamper((OMX_INDEXTYPE)OMX_Symbian_IndexConfigSceneMode);
            addIndexToDamperConfigList((OMX_INDEXTYPE)OMX_Symbian_IndexConfigSceneMode);
            //ZSLHDR related changes - Check for sceneMode HDR
            if(OMXCam->mSceneMode.eSceneType==(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR)
            {
                MSG0("HDR: Camera: HDR Scene was selected\n");
                OMXCam->previousScenetype=OMXCam->mSceneMode.eSceneType;
                s_scf_event event;
                event.type.other = 0;
                event.sig = START_HDR_SIG;
                iDeferredEventMgr.queuePriorEvent(&event);

                s_scf_event devent;
                devent.sig = Q_DEFERRED_EVENT_SIG;
                devent.type.other = 0;
                p_cam_sm->ProcessEvent(&devent);
            }
            else if((OMXCam->previousScenetype==(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR) &&
                       (OMXCam->mSceneMode.eSceneType!=(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR))
            {
                MSG0("HDR: Camera: Scene mode resume to previousScenetype\n");
                OMXCam->previousScenetype=OMXCam->mSceneMode.eSceneType;
                s_scf_event event;
                event.type.other=0;
                event.sig = RESUME_ZSL_SIG;
                iDeferredEventMgr.queuePriorEvent(&event);

                s_scf_event devent;
                devent.sig = Q_DEFERRED_EVENT_SIG;
                devent.type.other = 0;
                p_cam_sm->ProcessEvent(&devent);
            }
            OMXCam->previousScenetype=OMXCam->mSceneMode.eSceneType;
        }
        break;
        case OMX_Symbian_IndexConfigSharpness:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mSharpness);
            OMXCam->mCameraUserSettings.nSharpness = OMXCam->mSharpness.nValue;
            break;
        case OMX_Symbian_IndexConfigFlickerRemoval:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFlickerRemoval);
            configureDamper((OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlickerRemoval);
            addIndexToDamperConfigList((OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlickerRemoval);
            break;
        case OMX_Symbian_IndexConfigOrientationScene:
            /* Noting to do here */
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigCameraSelfTestSelect:
            OUTR(" ",OMX_ErrorNone); \
            OstTraceInt1(TRACE_FLOW, "Exit Camera::applyConfig (%d)", OMX_ErrorNone);
            break;

        case OMX_Symbian_IndexConfigCameraTesting:
        {
            if (OMX_TRUE == OMXCam->mTestingType.bEnabled)
            {
            event.type.other=0;
            event.sig = CAMERA_INDEX_CONFIG_SELFTEST_SIG;
            iDeferredEventMgr.queueNewEvent(&event);
            s_scf_event devent;
            devent.sig = Q_DEFERRED_EVENT_SIG;
            p_cam_sm->ProcessEvent(&devent);
            }
            OUTR(" ",OMX_ErrorNone);
			OstTraceInt1(TRACE_FLOW, "Exit Camera::applyConfig (%d)", OMX_ErrorNone);
            break;
        }
        case OMX_Symbian_IndexConfigBracketing:
        {
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mBracketing.config);
            // Store the config size in the opmode manager to setup the burst length.
            iOpModeMgr.SetBracketingCount(OMXCam->mBracketing.config.nNbrBracketingValues);
            break;
        }
		case IFM_IndexLatency_Hsmcamera:
			// Nothing to do
			break;
		case OMX_Symbian_IndexConfigAnalyzerFeedback:
		{
			OMX_SYMBIAN_FEEDBACKTYPE nFeedbackType = ((OMX_SYMBIAN_CONFIG_FEEDBACKTYPE*)pStructure)->nFeedbackType;
			OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pFeedback = NULL;
			// Search the corresponding feedbcak
			switch( nFeedbackType )
			{
				case OMX_SYMBIAN_FeedbackFaceTracker:
					pFeedback = &OMXCam->mAnalyzersFeedbacks.sFaceTracker.sFeedback;
					break;
				case OMX_SYMBIAN_FeedbackObjectTracker:
					pFeedback = &OMXCam->mAnalyzersFeedbacks.sObjectTracker.sFeedback;
					break;
				case OMX_SYMBIAN_FeedbackMotionLevel:
					pFeedback = &OMXCam->mAnalyzersFeedbacks.sMotionLevel.sFeedback;
					break;
				case OMX_SYMBIAN_FeedbackAutomaticSceneDetection:
					pFeedback = &OMXCam->mAnalyzersFeedbacks.sSceneDetection.sFeedback;
					break;
				default:
					// Search opaque feedback
					for(int i=0; i<OMXCam->mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived; i++) {
						if( nFeedbackType == OMXCam->mAnalyzersFeedbacks.sOpaque[i].sFeedback.nFeedbackType ) {
							pFeedback = &OMXCam->mAnalyzersFeedbacks.sOpaque[i].sFeedback;
							break;
						}
					}
					if( pFeedback == NULL ) {
						// Abnormal situation: OMX Camera should have stored feedback
						return OMX_ErrorInsufficientResources;
					}
					break;
			}
			// Send feedback to SW3A
			err = i3AComponentManager.setConfig(nIndex, (OMX_PTR)pFeedback);
			break;
		}
		case OMX_STE_IndexConfigCommonZoomMode:
			OMXCam->mZoomMode.bEnabled = ((OMX_CONFIG_BOOLEANTYPE *)pStructure)->bEnabled;
			MSG1("applyConfig. [smooth zoom] Smooth Zoom = %s\n", (OMXCam->mZoomMode.bEnabled == OMX_TRUE)?"ENABLED":"DISABLED");
			break;
        case OMX_STE_IndexConfigMeteringOn:
            err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mMeteringOn);
            break;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
      case OMX_3A_IndexConfigIspParameters:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mIspParameters);
          break;
      case OMX_3A_IndexConfigExposure:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mExposure);
          break;
      case OMX_3A_IndexConfigGamma:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mGamma);
          break;
      case OMX_3A_IndexConfigLsc:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mLsc);
          break;
      case OMX_3A_IndexConfigLinearizer:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mLinearizer);
          break;
      case OMX_3A_IndexConfigFocus:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFocus);
          break;
      case OMX_3A_IndexConfigFlashSynchro:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mFlashSynchro);
          break;
      case OMX_3A_IndexConfigMaxFramerate:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mMaxFramerate);
          break;
      case OMX_3A_IndexConfigSensorMode:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mSensorModeparam);
          break;
      case OMX_3A_IndexConfigEEPROMData:
          err = i3AComponentManager.setConfig(nIndex, (OMX_PTR) &OMXCam->mEepromdata);
          break;
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
      case OMX_STE_IndexConfigExifTimestamp:
          iExifTimeStamp = ((OMX_TIME_CONFIG_TIMESTAMPTYPE *)pStructure)->nTimestamp;
          break;
		default :
			err = OMX_ErrorNone;
	}
	OUTR(" ",err);
	OstTraceInt1(TRACE_FLOW, "Exit Camera::applyConfig (%d)", err);
	return err;
}

/* */
/* Camera::start */
/* */
OMX_ERRORTYPE Camera::start() {
    IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::start");
    OMX_ERRORTYPE omxerr = OMX_ErrorNone;

  iIspctlCom.setPanic(OMX_FALSE); 

    /*
    * Start the Tuning Loader
    */
    TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
    tlmErr = iTuningLoaderManager.start();
    if (tlmErr != TUNING_LOADER_MGR_OK){
        DBGT_ERROR("Error: iTuningLoaderManager.start() err=%d (%s)\n", tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
        OUTR(" ",(OMX_ErrorInsufficientResources));
		OstTraceFiltInst1(TRACE_ERROR, "Error: iTuningLoaderManager.start() err=%d", tlmErr);
		OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::start (%d)", tlmErr);
        return OMX_ErrorInsufficientResources;
    }

    /* First, initialises the board */
    /* board & sensor init */
    /* tell coverity that GetCameraSlot return value does not need to be checked */
    /* coverity[check_return : FALSE] */
    omxerr = iSiaPlatformManager.init(iSensor->GetCameraSlot());
    if(omxerr != OMX_ErrorNone) {
        return omxerr;
    }

    /* starts ISPCTL NMF component
     * loads ISP FW
     * */
    omxerr = iIspctlComponentManager.startNMF();
    if (omxerr != OMX_ErrorNone){
        OUTR(" ",(omxerr));
        OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::start (%d)", omxerr);
        return omxerr;
    }

    /* starts ImgNetwork NMF Component
     * */
    mpImgNetwork->start();

    /* starts timer NMF Component
     * */
    pTimer->start();

#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
    /* starts asyncflash NMF Component
     * */
    pAsyncflash->start();
#endif

/* Then sets the appropriate IF to the appropriate internal objects
 *
 */
    /* sets the command IF from ISPCTL NMF to the IspctlCom object */

    iIspctlCom.setIspctlCmd(iIspctlComponentManager.getIspctlCmd());

    /* sets the mIgrabctlConfigure IF from ImgNetwork to the GrabControl object */
    iGrabControl.setConfigureControl(mIgrabctlConfigure);

    /* sets the mIstabConfigure IF from ImgNetwork to the GrabControl object */
    iGrabControl.setStabConfigure(mIstabConfigure);

    /* iSiaPlatformManager uses the ispctl configure interface so it is passed from iIspctlComponentManager to iSiaPlatformManager */
    iSiaPlatformManager.setIspctlCfg(iIspctlComponentManager.getIspctlCfg());

	
    camport * port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
    OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
    mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
    mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
/* Then configure the ImgNetwork
 *
 */
    /* configure the port/pipe mapping */
 if(mParamPortDefinition.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
		 {
		 	setPipes(GRBPID_PIPE_LR,GRBPID_PIPE_HR,GRBPID_PIPE_HR);
		 	MSG1( "Camera::Pipes: (%d)", GRBPID_PIPE_HR);
		 }
		 else
		 {
		    setPipes(GRBPID_PIPE_LR,GRBPID_PIPE_RAW_OUT,GRBPID_PIPE_HR);
		    MSG1( "Camera::Pipes: (%d)", GRBPID_PIPE_RAW_OUT); 
		 }

#ifndef ENABLE_LOWPOWER_DSP
	iGrab.disableDspFineLowPower();
#endif

    /* tell DSP the CCP/CSI config */
    /* tell coverity that GetCameraSlot return value does not need to be checked */
    /* coverity[check_return : FALSE] */
    iSiaPlatformManager.config(iSensor->GetCameraSlot());

    IFM_CONFIG_ISPSHARED res;
    res.nSize = sizeof(res);
    getOmxIlSpecVersion(&(res.nVersion));
    res.nPortIndex  = port->getTunneledPort();

    //if(port->getTunneledComponent()) {
    //  OMX_ERRORTYPE error = OMX_GetConfig(port->getTunneledComponent(),(OMX_INDEXTYPE)(IFM_IndexIsISPShared),&res);
    //  if(error != OMX_ErrorUnsupportedIndex)
            iResourceSharerManager.startNMF(); // no error returned
    //}

    /* Create and map the xp70/ARM shared memory buffer.
     * Pre-condition: the ISP must be powered on and clocked. */
    camSharedMemError_t csm_err = iSharedMemory.create();
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Error creating the xp70/ARM shared buffer: %d", (int) csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Error creating the xp70/ARM shared buffer: %d", (int) csm_err);
        return OMX_ErrorInsufficientResources;
    }

    // Get the sensore modes Shared Memory chunk
    csm_err = iSharedMemory.getChunk(&iSensorModesShmChunk, CAM_SHARED_MEM_CHUNK_SENSOR_MODES);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the sensor modes shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the sensor modes shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    // Get the NVM Shared Memory chunk
    csm_err = iSharedMemory.getChunk(&iNvmShmChunk, CAM_SHARED_MEM_CHUNK_SENSOR_NVM);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the NVM shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the NVM shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    // Get the Firmware Shared Memory chunk
    csm_err = iSharedMemory.getChunk(&iFwShmChunk, CAM_SHARED_MEM_CHUNK_FW_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the firmware shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the firmware shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaSharpGreen_LR, CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_LR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the LR Green Sharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the LR Green Sharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaSharpRed_LR, CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_LR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the LR Red Sharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the LRRed Sharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaSharpBlue_LR, CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_LR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the LR Blue Sharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the LR Blue Sharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaUnSharpGreen_LR, CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_LR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the LR Green UnSharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the LR Green UnSharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaUnSharpRed_LR, CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_LR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the LR Red UnSharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the LR Red UnSharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaUnSharpBlue_LR, CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_LR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the LR Blue UnSharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the LR Blue UnSharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaSharpGreen_HR, CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_HR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the HR Green Sharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the HR Green Sharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaSharpRed_HR, CAM_SHARED_MEM_CHUNK_SHARP_RED_GAMMA_HR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the HR Red Sharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the HR Red Sharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaSharpBlue_HR, CAM_SHARED_MEM_CHUNK_SHARP_BLUE_GAMMA_HR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the HR Blue Sharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the HR Blue Sharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaUnSharpGreen_HR, CAM_SHARED_MEM_CHUNK_UNSHARP_GREEN_GAMMA_HR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the HR Green UnSharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the HR Green UnSharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaUnSharpRed_HR, CAM_SHARED_MEM_CHUNK_UNSHARP_RED_GAMMA_HR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the HR Red UnSharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the HR Red UnSharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    csm_err = iSharedMemory.getChunk(&iGammaUnSharpBlue_HR, CAM_SHARED_MEM_CHUNK_UNSHARP_BLUE_GAMMA_HR_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not get the HR Blue UnSharp Gamma shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the HR Blue UnSharp Gamma shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    // Start Sw3A component
    omxerr = i3AComponentManager.start();
    if( omxerr != OMX_ErrorNone) {
        DBGT_ERROR("Could not get the NVM shared memory chunk, err=%d\n", csm_err);
		OstTraceFiltInst1(TRACE_ERROR, "Could not get the NVM shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return omxerr;
    }

    
	COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;
	pOmxCamera->mHiddenBuffersInfo.physicalBuffersAddress=NULL;
	pOmxCamera->mHiddenBuffersInfo.hiddenBMSBuffers=NULL;
	pOmxCamera->mHiddenBuffersInfo.totalBuffers=0;		
    // start checkalive timer
    // checkAliveStartStop();

    OUTR(" ",OMX_ErrorNone);
    OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::start (%d)", OMX_ErrorNone);
    return OMX_ErrorNone;
}


/* */
/* Camera::instantiate */
/* */
OMX_ERRORTYPE Camera::instantiate() {
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::instantiate");
    OMX_ERRORTYPE error = OMX_ErrorNone;
    t_nmf_error nmf_err = NMF_OK;

    /*
     * Tuning Loader construction
     */
    TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
    MSG0("Constructing the Tuning Loader Manager\n");
    OstTraceFiltInst0(TRACE_DEBUG, "Constructing the Tuning Loader Manager");
    tlmErr = iTuningLoaderManager.construct(mENSComponent.getOMXHandle());
    if (tlmErr != TUNING_LOADER_MGR_OK){
        DBGT_ERROR("Error: iTuningLoaderManager.construct() err=%d (%s)\n", tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceFiltInst1(TRACE_ERROR, "Error: iTuningLoaderManager.construct() err=%d", tlmErr);
        OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::instantiate (%d)", tlmErr);
        return OMX_ErrorInsufficientResources;
    }

    /*
     *  Bind to the Tuning Data Base
     */
    pTuningDataBase = CTuningDataBase::getInstance(CAMERA_TUNING_DATABASE_CLIENT_NAME);
    if(pTuningDataBase == NULL) {
        DBGT_ERROR("Error: could not bind to the Tuning Data Base: pTuningDataBase==NULL\n");
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceFiltInst0(TRACE_ERROR, "Error: could not bind to the Tuning Data Base: pTuningDataBase==NULL");
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorInsufficientResources");
        return OMX_ErrorInsufficientResources;
    }
    MSG1("Bound to Tuning Data Base: pTuningDataBase=%p\n", pTuningDataBase);
    OstTraceFiltInst1(TRACE_DEBUG, "Bound to Tuning Data Base: pTuningDataBase=0x%X", (t_uint64)pTuningDataBase);

    /* SW3A component manager */
    OMX_ERRORTYPE omxerr = i3AComponentManager.instantiate(mENSComponent.getOMXHandle());
    if (omxerr != OMX_ErrorNone) {
        OUTR(" ",(omxerr));
        OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::instantiate (%d)", omxerr);
        return omxerr;
    }

    /* Flash driver and SW3A related inits */
    iFlashDriver = CFlashDriver::Open();
    if (NULL == iFlashDriver){
        DBGT_ERROR("Error: could not open flash driver.\n");
        OstTraceFiltInst0(TRACE_ERROR, "Error: could not open flash driver.");
        DBC_ASSERT(0);
    }

    /* Instantiate and bind ISPCTL NMF Component */
    iIspctlComponentManager.setOMXHandle(mENSComponent.getOMXHandle());
    error = iIspctlComponentManager.instantiateNMF();
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error: iIspctlComponentManager.instantiateNMF() 0x%X\n", error);
        OUTR(" ",(error));
        OstTraceFiltInst1(TRACE_ERROR, "Error: iIspctlComponentManager.instantiateNMF() 0x%X", error);
        OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::instantiate (%d)", error);
        return error;
     }

    /* Instantiate ImgNetwork */
    mpImgNetwork = img_networkCreate();
    if (NULL == mpImgNetwork) {
        DBGT_ERROR("Error: img_networkCreate() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: img_networkCreate() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    mpImgNetwork->domain = mENSComponent.getNMFDomainHandle();
    nmf_err = mpImgNetwork->construct();
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: mpImgNetwork->construct() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: mpImgNetwork->construct() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }
    /* bind provided interfaces */
    nmf_err = mpImgNetwork->bindFromUser("sendcommand", 5, &mIsendCommand);
    nmf_err |= mpImgNetwork->bindFromUser("fsminit", 1, &mIfsmInit);
    nmf_err |= mpImgNetwork->bindFromUser("fillthisbuffer[0]",
                                          16,
                                          &mIfillThisBuffer[0]);
    nmf_err |= mpImgNetwork->bindFromUser("fillthisbuffer[1]",
                                          16,
                                          &mIfillThisBuffer[1]);
    nmf_err |= mpImgNetwork->bindFromUser("fillthisbuffer[2]",
                                          16,
                                          &mIfillThisBuffer[2]);
    nmf_err |= mpImgNetwork->bindFromUser("grabctl_configure", 8, &mIgrabctlConfigure);

    nmf_err |= mpImgNetwork->bindFromUser("stab_configure", 2, &mIstabConfigure);
    nmf_err |= mpImgNetwork->bindFromUser("grab_command", 1, &iGrab.mGrabCmd);

    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: mpImgNetwork->bindFromUser() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: mpImgNetwork->bindFromUser() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    /* bind callbacks */
    OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();

    nmf_err |= EnsWrapper_bindToUser(handle, mpImgNetwork, "proxy",
                                     this->getEventHandlerCB(), 8);
    nmf_err |= EnsWrapper_bindToUser(handle, mpImgNetwork, "fillbufferdone[0]",
                                     &iCallback,
                                     16);
    nmf_err |= EnsWrapper_bindToUser(handle, mpImgNetwork, "fillbufferdone[1]",
                                     &iCallback,
                                     16);
    nmf_err |= EnsWrapper_bindToUser(handle, mpImgNetwork, "fillbufferdone[2]",
                                     &iCallback,
                                     16);
    nmf_err |= EnsWrapper_bindToUser(handle, mpImgNetwork, "grabctl_acknowledges",
                                     (grabctl_api_acknowledgesDescriptor *)this, 16);
    nmf_err |= EnsWrapper_bindToUser(handle, mpImgNetwork, "grabctl_error",
                                     (grabctl_api_errorDescriptor *)this, 16);
    nmf_err |= EnsWrapper_bindToUser(handle, mpImgNetwork, "grab_alert", (grab_api_alertDescriptor*)this, 4);
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: mpImgNetwork->EnsWrapper_bindToUser() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: mpImgNetwork->EnsWrapper_bindToUser() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

#ifdef CAMERA_ENABLE_OMX_STATS_PORT
  pExtItf = Get3aExtInterface();
  pExtItf->SetCallback(pExtItf->handler, this, statsCallback);
#endif // CAMERA_ENABLE_OMX_STATS_PORT
    /* Instantiate Timer */
    pTimer = timerWrappedCreate();
    if (NULL == pTimer) {
        DBGT_ERROR("Error: timerWrappedCreate() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: timerWrappedCreate() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    nmf_err = pTimer->construct();
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pTimer->construct() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: pTimer->construct() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    nmf_err = pTimer->getInterface("timer", &mTimer);
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pTimer->bindFromUser() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: pTimer->bindFromUser() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    nmf_err = EnsWrapper_bindToUser(handle, pTimer, "alarm", (timer_api_alarmDescriptor*)this, 4);
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pTimer->EnsWrapper_bindToUser() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: pTimer->EnsWrapper_bindToUser() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    /* Instantiate ResourceSharer */
    iResourceSharerManager.setOMXHandle(mENSComponent.getOMXHandle());

    error = iResourceSharerManager.instantiateNMF();
    if(error!= OMX_ErrorNone)
    {
        DBGT_ERROR("iResourceSharerManager.instantiateNMF 0x%X\n", error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "iResourceSharerManager.instantiateNMF 0x%X", error);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }


    error = iResourceSharerManager.bindNMF();
    if(error!= OMX_ErrorNone)
    {
        DBGT_ERROR("iResourceSharerManager.bindNMF 0x%X\n", error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "iResourceSharerManager.bindNMF 0x%X", error);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }


#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
    /* Instantiate asyncflash */
    pAsyncflash = asyncflash_wrpCreate();
    if(NULL == pAsyncflash)
    {
        DBGT_ERROR("asyncflash_wrpCreate 0x%X\n", error);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "asyncflash_wrpCreate 0x%X", error);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    nmf_err = pAsyncflash->construct();
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pAsyncflash->construct() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: pAsyncflash->construct() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    nmf_err = pAsyncflash->bindFromUser("request", 1, &mAsyncflashRequest);
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pAsyncflash->bindFromUser() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: pAsyncflash->bindFromUser() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    nmf_err = EnsWrapper_bindToUser(handle, pAsyncflash, "response", (asyncflash_api_responseDescriptor*)this, 1);
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pAsyncflash->EnsWrapper_bindToUser() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "Error: pAsyncflash->EnsWrapper_bindToUser() 0x%X", nmf_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }
#endif

    /* Construct the ISP dampers */
    t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;
    dErr = iIspDampers.Construct();
    if(dErr!=ISP_DAMPERS_OK) {
        DBGT_ERROR("Error: iIspDampers.Construct() err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceFiltInst1(TRACE_ERROR, "Error: iIspDampers.Construct() err=%d", dErr);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::instantiate OMX_ErrorInsufficientResources");
        return OMX_ErrorInsufficientResources;
    }


    OUTR(" ",(error));
    OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::instantiate (%d)", error);
    return error;
}


/* */
/* Camera::stop */
/* */
OMX_ERRORTYPE Camera::stop() {
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::stop");
    OMX_ERRORTYPE omxerr = OMX_ErrorNone;

  iIspctlCom.setPanic(OMX_TRUE); // protect any interface that may be called from this point on 

	COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;
	if(pOmxCamera->mHiddenBuffersInfo.hiddenBMSBuffers!=NULL)
	{
		omxerr = MMHwBuffer::Destroy(pOmxCamera->mHiddenBuffersInfo.hiddenBMSBuffers);
		if (omxerr != OMX_ErrorNone) 
		{
			MSG1("Problem freeing BMS/BML SDRAM buffer (err=%d)\n", omxerr);
			OstTraceFiltStatic1(TRACE_ERROR, "Problem freeing BMS/BML SDRAM buffer (err=%d)", (&mENSComponent), omxerr);
			DBC_ASSERT(0);
		}
		pOmxCamera->mHiddenBuffersInfo.hiddenBMSBuffers=NULL;
  	}
	if(pOmxCamera->mHiddenBuffersInfo.physicalBuffersAddress!=NULL)
	{
		free(pOmxCamera->mHiddenBuffersInfo.physicalBuffersAddress);
		pOmxCamera->mHiddenBuffersInfo.physicalBuffersAddress=NULL;
  	}
	
    if(iFlashDriver){
      iFlashDriver->Close();
      iFlashDriver = NULL;
    }

    /* Stop sw3A */
    /* Open was done in boot statemachine, but as long as ENS does not allow to have a shutdown state machine,
     * we have to close() here */
    omxerr = i3AComponentManager.close();
    if (omxerr != OMX_ErrorNone){
        OUTR(" ",(omxerr));
        OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::stop (%d)", omxerr);
        return omxerr;
    }
    omxerr = i3AComponentManager.stop();
    if (omxerr != OMX_ErrorNone){
        OUTR(" ",(omxerr));
        OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::stop (%d)", omxerr);
        return omxerr;
    }

    // Release the SensorModes Shared Memory chunk
    camSharedMemError_t csm_err = CAM_SHARED_MEM_ERR_NONE;
    csm_err = iSharedMemory.releaseChunk(CAM_SHARED_MEM_CHUNK_SENSOR_MODES);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not release the sensor modes shared memory chunk, err=%d\n", csm_err);
        OstTraceFiltInst1(TRACE_ERROR, "Could not release the sensor modes shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    // Release the NVM Shared Memory chunk
    csm_err = CAM_SHARED_MEM_ERR_NONE;
    csm_err = iSharedMemory.releaseChunk(CAM_SHARED_MEM_CHUNK_SENSOR_NVM);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not release the NVM shared memory chunk, err=%d\n", csm_err);
        OstTraceFiltInst1(TRACE_ERROR, "Could not release the NVM shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }

    // Release the Firmware Shared Memory chunk
    csm_err = iSharedMemory.releaseChunk(CAM_SHARED_MEM_CHUNK_FW_CONFIG);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not release the firmware shared memory chunk, err=%d\n", csm_err);
        OstTraceFiltInst1(TRACE_ERROR, "Could not release the firmware shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }
for(int i =CAM_SHARED_MEM_CHUNK_SHARP_GREEN_GAMMA_LR_CONFIG;i<CAM_SHARED_MEM_CHUNK_MAX;i++)
{
        // Release the Gamma Shared Memory chunk
    csm_err = iSharedMemory.releaseChunk((camSharedMemChunkId_t)i);
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Could not release the firmware shared memory chunk, err=%d\n", csm_err);
        OstTraceFiltInst1(TRACE_ERROR, "Could not release the firmware shared memory chunk, err=%d", csm_err);
        DBC_ASSERT(csm_err != CAM_SHARED_MEM_ERR_NONE);
        return OMX_ErrorInsufficientResources;
    }
}

    /* Destroy and unmap the xp70/ARM shared memory buffer
     * Pre-condition: the ISP must be powered on and clocked. */
    csm_err = iSharedMemory.destroy();
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
        DBGT_ERROR("Error: iSharedMemory.destroy 0x%X\n", (int) csm_err);
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceFiltInst1(TRACE_ERROR, "Error: iSharedMemory.destroy 0x%X", (int) csm_err);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::stop OMX_ErrorInsufficientResources");
        return OMX_ErrorInsufficientResources;
    }

    /* board & sensor deinit */
    /* tell coverity that GetCameraSlot return value does not need to be checked */
    /* coverity[check_return : FALSE] */
    omxerr=iSiaPlatformManager.deinit(iSensor->GetCameraSlot());
    if(omxerr != OMX_ErrorNone) {
        return omxerr;
    }

    /* Stop ISPCTL */
    omxerr= iIspctlComponentManager.stopNMF();
    if(omxerr != OMX_ErrorNone)
        return omxerr;
    /* Stop ImgNetwork */
    mpImgNetwork->stop_flush();

    // stop checkalive timer if running
    mTimer.stopTimer();
    iCheckAliveTimerRunning = false;
    iCheckTimerRunning = false;

    /* Stop itmer */
    pTimer->stop_flush();

#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
    /* Stop asyncflash */
    pAsyncflash->stop_flush();
#endif

    omxerr =    iResourceSharerManager.stopNMF();
    if(omxerr!= OMX_ErrorNone)
    {
        DBGT_ERROR("iResourceSharerManager.stopNMF 0x%X\n", omxerr);
        OUTR(" ",(OMX_ErrorHardware));
        OstTraceFiltInst1(TRACE_ERROR, "iResourceSharerManager.stopNMF 0x%X", omxerr);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::stop OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    // Stop the tuning loader
    TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
    tlmErr = iTuningLoaderManager.stop();
    if (tlmErr != TUNING_LOADER_MGR_OK){
        DBGT_ERROR("Error: iTuningLoaderManager.stop() err=%d (%s)\n", tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
        OUTR(" ",(OMX_ErrorUndefined));
        OstTraceFiltInst1(TRACE_ERROR, "Error: iTuningLoaderManager.stop() err=%d", tlmErr);
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::stop OMX_ErrorUndefined");
        return OMX_ErrorUndefined;
    }

    OUTR(" ",(OMX_ErrorNone));
    OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::stop (%d)", OMX_ErrorNone);
    return OMX_ErrorNone;
}


/* */
/* Camera::configure */
/* */
OMX_ERRORTYPE Camera::configure() {
    OMX_ERRORTYPE omxerr = OMX_ErrorNone;
    return omxerr;
}

void Camera::grababorted(t_uint16 port_idx){
	ASYNC_IN0("");
	s_scf_event event;
	event.sig = GRAB_ABORTED_SIG;
	event.type.other = port_idx;
	iDeferredEventMgr.queuePriorEvent(&event);

	s_scf_event devent;
	devent.sig = Q_DEFERRED_EVENT_SIG;
	p_cam_sm->ProcessEvent(&devent);

	ASYNC_OUT0("");
}




/* */
/* Camera::doSpecificSendCommand is used to delay the SendCommand */
/* */
OMX_ERRORTYPE Camera::doSpecificSendCommand(OMX_COMMANDTYPE cmd, OMX_U32 nParam, OMX_BOOL &bDeferredCmd)
{
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::doSpecificSendCommand");
    OMX_STATETYPE currentState = OMX_StateInvalid;
    bDeferredCmd = OMX_FALSE;
    mENSComponent.GetState(&mENSComponent, &currentState);
    MSG("%s cmd=%d nparam=%lu currentstate=%d(%s)\n" ,__FUNCTION__, cmd,nParam,currentState,
          CError::stringOMXstate((OMX_STATETYPE)currentState));
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
  if(((cmd==OMX_CommandPortEnable)||(cmd==OMX_CommandPortDisable)||(cmd==OMX_CommandFlush)) &&
       (nParam==CAMERA_PORT_STATS_AEWB || nParam==CAMERA_PORT_STATS_AF))
  {
    bDeferredCmd = OMX_TRUE;
    s_scf_event event;
    event.type.other=0;
    if (cmd==OMX_CommandFlush)
    {
      pExtItf->FlushPort(pExtItf->handler, nParam);
      mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandFlush, nParam);
    }
    else if (cmd==OMX_CommandPortDisable)
    {
      mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandPortDisable, nParam);
    }
  }
  else
#endif // CAMERA_ENABLE_OMX_STATS_PORT
    if(((cmd == OMX_CommandStateSet)&&(nParam==OMX_StateIdle)&&(currentState == OMX_StateExecuting))
        ||((cmd ==OMX_CommandPortEnable)&&(currentState == OMX_StateExecuting))
        ||((cmd ==OMX_CommandPortDisable)&&(currentState == OMX_StateExecuting)))
    {
        bDeferredCmd = OMX_TRUE;

        s_scf_event event;
        event.type.other=0;

        if((nParam==OMX_ALL) && ((cmd ==OMX_CommandPortEnable)||(cmd ==OMX_CommandPortDisable)))
        {
            camport * port0 = (camport *) mENSComponent.getPort(0);
            camport * port1 = (camport *) mENSComponent.getPort(1);
            camport * port2 = (camport *) mENSComponent.getPort(2);
            if(cmd ==OMX_CommandPortEnable)
            {
                /* TODO: Currently we don't handle the case of settings change on OMX_ALL */
                DBC_ASSERT((port0->portSettingsChanged==OMX_FALSE)&&(port1->portSettingsChanged==OMX_FALSE)&&(port2->portSettingsChanged==OMX_FALSE));
            }
            event.sig = SEND_COMMAND_SIG;
        }
        else
        {
            camport * port = (camport *) mENSComponent.getPort(nParam); //in such case, nParam is the portIndex
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
            if((cmd ==OMX_CommandPortEnable) &&
                    ((port->portSettingsChanged==OMX_TRUE) ||
                     i3AComponentManager.bSensorModechanged))
            {
                event.sig = ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG;
            }
#else
            if((cmd ==OMX_CommandPortEnable)&&(port->portSettingsChanged==OMX_TRUE)) {
                /* To avoid a wrong framerate setting, forward port enable
                 * signal even if port settings are not changed.*/
                event.sig = ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG;
            }
#endif
            else {
                /* no new settings to be applied */
                event.sig = SEND_COMMAND_SIG;
            }
        }

        event.args.sendCommand.Cmd =cmd;
        event.args.sendCommand.nParam = nParam;
        iDeferredEventMgr.queueNewEvent(&event);
        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_cam_sm->ProcessEvent(&devent);

        /*TODO : What happens when goBackIdle is triggered when record is not ended */

    }
    OUTR(" ",(OMX_ErrorNone));
    OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::doSpecificSendCommand (%d)", OMX_ErrorNone);
    return OMX_ErrorNone;
}



/* */
/* Camera::deInstantiate */
/* */
OMX_ERRORTYPE Camera::deInstantiate() {
    IN0("\n");
	OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::deInstantiate");
    OMX_ERRORTYPE error = OMX_ErrorNone;
    t_nmf_error nmf_err = NMF_OK;


    OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();

#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
    /* deinstantiate asyncflash */
    nmf_err = pAsyncflash->unbindFromUser("request");
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pAsyncflash unbindfromUser 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: pAsyncflash unbindfromUser 0x%X", nmf_err);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }
    nmf_err = EnsWrapper_unbindToUser(handle, pAsyncflash, "response");
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pAsyncflash->unbindToUser 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: pAsyncflash->unbindToUser 0x%X", nmf_err);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }
    nmf_err = pAsyncflash->destroy();
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: pAsyncflash->destroy() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: pAsyncflash->destroy() 0x%X", nmf_err);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }
    asyncflash_wrpDestroy(pAsyncflash);
#endif

	nmf_err = EnsWrapper_unbindToUser(handle, pTimer, "alarm");
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: pTimer->unbindToUser 0x%X\n", nmf_err);
		OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: pTimer->unbindToUser 0x%X", nmf_err);
		OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::deInstantiate (%d)", OMX_ErrorHardware);
		return OMX_ErrorHardware;
		}
	nmf_err = pTimer->destroy();
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: pTimer->destroy() 0x%X\n", nmf_err);
		OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: pTimer->destroy() 0x%X", nmf_err);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
		return OMX_ErrorHardware;
	}
	timerWrappedDestroy(pTimer);


    /* deinstantiate Resource Sharer*/
    error = iResourceSharerManager.deinstantiateNMF();
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error: iResourceSharerManager.deinstantiateNMF() 0x%X\n", error);
        OUTR(" ",(error));
		OstTraceFiltInst1(TRACE_ERROR, "Error: iResourceSharerManager.deinstantiateNMF() 0x%X", error);
		OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::deInstantiate (%d)", error);
        return error;
     }

   error = i3AComponentManager.deInstantiate(handle);
   if (error != OMX_ErrorNone){
      OUTR(" ",(error));
		OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::deInstantiate (%d)", error);
      return error;
   }

    /* Destroy the ISP dampers */
    t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;
    dErr = iIspDampers.Destroy();
    if(dErr!=ISP_DAMPERS_OK) {
        DBGT_ERROR("Error: iIspDampers.Destroy() err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
        OUTR(" ", OMX_ErrorHardware);
		OstTraceFiltInst1(TRACE_ERROR, "Error: iIspDampers.Destroy() err=%d", dErr);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

   /* Destroy the tuning loader */
   TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
   MSG0("Destroying the Tuning Loader Manager\n");
   OstTraceFiltInst0(TRACE_DEBUG, "Destroying the Tuning Loader Manager");
   tlmErr = iTuningLoaderManager.destroy();
   if (tlmErr != TUNING_LOADER_MGR_OK){
       DBGT_ERROR("Error: iTuningLoaderManager.destroy() err=%d (%s)\n", tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
       OUTR(" ", OMX_ErrorHardware);
		OstTraceFiltInst1(TRACE_ERROR, "Error: iTuningLoaderManager.destroy() err=%d", tlmErr);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
       return OMX_ErrorHardware;
   }

    /* Unbind from the Tuning Data Base */
    if(pTuningDataBase != NULL) {
        pTuningDataBase->releaseInstance(CAMERA_TUNING_DATABASE_CLIENT_NAME);
        MSG1("Unbound from Tuning Data Base: pTuningDataBase=%p\n", pTuningDataBase);
		OstTraceFiltInst1(TRACE_DEBUG, "Unbound from Tuning Data Base: pTuningDataBase=0x%X", (t_uint64)pTuningDataBase);
        pTuningDataBase = NULL;
    }

    /* free remaining buffers (doing it in destructor is too late for RME) */
    error = iGrabControl.freeGrabCache();
    if (error != OMX_ErrorNone) {
        return error;
    }
    error = iGrabControl.freeStabBuffers();
    if (error != OMX_ErrorNone) {
        return error;
    }

    /* deinstantiate ISPCTL */
    error = iIspctlComponentManager.deinstantiateNMF();
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error: iIspctlComponentManager.deinstantiateNMF() 0x%X\n", error);
        OUTR(" ",(error));
		OstTraceFiltInst1(TRACE_ERROR, "Error: iIspctlComponentManager.deinstantiateNMF() 0x%X", error);
		OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::deInstantiate (%d)", error);
        return error;
     }


    /* deinstantiate NMF Network*/
    nmf_err |= mpImgNetwork->unbindFromUser("sendcommand");
    nmf_err |= mpImgNetwork->unbindFromUser("fsminit");
    nmf_err |= mpImgNetwork->unbindFromUser("fillthisbuffer[0]");
    nmf_err |= mpImgNetwork->unbindFromUser("fillthisbuffer[1]");
    nmf_err |= mpImgNetwork->unbindFromUser("fillthisbuffer[2]");
    nmf_err |= mpImgNetwork->unbindFromUser("grabctl_configure");
    nmf_err |= mpImgNetwork->unbindFromUser("grab_command");
    nmf_err |= mpImgNetwork->unbindFromUser("stab_configure");
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: img_network unbindfromUser 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: img_network unbindfromUser 0x%X", nmf_err);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "proxy");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[0]");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[1]");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[2]");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_acknowledges");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_error");
    nmf_err |= EnsWrapper_unbindToUser(handle, mpImgNetwork, "grab_alert");
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: img_network unbindToUser 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: img_network unbindToUser 0x%X", nmf_err);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }

#ifdef CAMERA_ENABLE_OMX_STATS_PORT
  pExtItf = NULL;
#endif // CAMERA_ENABLE_OMX_STATS_PORT

    nmf_err = mpImgNetwork->destroy();
    if (nmf_err != NMF_OK) {
        DBGT_ERROR("Error: mImgNetwork->destroy() 0x%X\n", nmf_err);
        OUTR(" ",(OMX_ErrorHardware));
		OstTraceFiltInst1(TRACE_ERROR, "Error: mImgNetwork->destroy() 0x%X", nmf_err);
		OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::deInstantiate OMX_ErrorHardware");
        return OMX_ErrorHardware;
    }
    img_networkDestroy(mpImgNetwork);

    /* exit from SM Framework */
    s_scf_event devent;
    devent.sig = Q_FINAL_SIG;
    p_cam_sm->ProcessEvent(&devent);
    destroySM();

    OUTR(" ",(error));
	OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::deInstantiate (%d)", error);
    return error;
}

/* This function is run after check alive trace parameters
 * are updated. It checks the parameters & starts / stops the timer
 */
void Camera::checkAliveStartStop() {
    if (!iCheckAliveTimerRunning){
        if (mTimer.startTimer(1000*1000, 1000*1000)) {
            MSG0("check-alive 1s timer startup failed\n");
        	OstTraceFiltInst0(TRACE_DEBUG, "check-alive 1s timer startup failed");
        } else {
            MSG0("check-alive 1s timer started\n");
        	OstTraceFiltInst0(TRACE_DEBUG, "check-alive 1s timer started");
            iCheckAliveTimerRunning = true;
        }
  } else {
    iCheckAliveTimerRunning = false;
  }
  if (!iCheckIntervalMsec && iPrintStatus) {
    // just print the status once if flag is on
    printCheckAliveStatus();
  }
}

/* This function prints status of camera state machine to debug
 * stream.
 */
void Camera::printCheckAliveStatus() {
  const char* opmode=NULL;
  switch (iOpModeMgr.CurrentOperatingMode) {
  case OpMode_Cam_VideoPreview:
    opmode = "VideoPreview";
    break;
  case OpMode_Cam_VideoRecord:
    opmode = "VideoRecord";
    break;
  case OpMode_Cam_StillPreview:
    opmode = "StillPreview";
    break;
  case OpMode_Cam_StillCaptureSingle:
    opmode = "StillCaptureSingle";
    break;
  case OpMode_Cam_StillCaptureBurstLimited:
    opmode = "StillCaptureBurstLimited";
    break;
  case OpMode_Cam_StillCaptureBurstInfinite:
    opmode = "StillCaptureBurstInfinite";
    break;
  case OpMode_Cam_StillFaceTracking:
    opmode = "StillFaceTracking";
    break;
  case OpMode_Cam_StillCaptureSingleInVideo:
    opmode = "StillCaptureSingleInVideo";
    break;
  case OpMode_Cam_StillCaptureBurstLimitedInVideo:
    opmode = "StillCaptureBurstLimitedInVideo";
    break;
  case OpMode_Cam_StillCaptureBurstInfiniteInVideo:
    opmode = "StillCaptureBurstInfiniteInVideo";
    break;
  default:
    opmode = "UNKNOWN";
    break;
  }

  MSG0("Camera check-alive status:\n");
  if(opmode != NULL) // To avoid a compiletime warning
  {
	MSG1("Camera op-mode = %s\n", opmode);
  }
  OstTraceFiltInst0(TRACE_DEBUG, "Camera check-alive status:");
  //OstTraceFiltInst1(TRACE_DEBUG, "Camera op-mode = %s", opmode);
  if (p_cam_sm) {
    p_cam_sm->printStatus();
  }
}

/** ISP info callback hook implementation */
static void Camera_ispctlInfo(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd)
{
    ASYNC_IN0("\n");

    COmxCamera* omxcam = (COmxCamera*)ctxtHnd;
    Camera* cam = (Camera*)&omxcam->getProcessingComponent();

    OstTraceFiltStatic0(TRACE_FLOW, "Entry Camera_ispctlInfo", cam);

    if (OMX_TRUE == omxcam->mExposureInitiatedClbkReq.bEnable)
    {
        omxcam->eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, OMX_ALL, OMX_Symbian_IndexConfigExposureInitiated, NULL);

        //performance traces start
        cam->p_cam_sm->latencyMeasure(&(cam->p_cam_sm->mTime));
        cam->mlatency_Hsmcamera.e_OMXCAM_ShutterReleaseLag.t0 = cam->p_cam_sm->mTime;
		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShutterReleaseLag %d", 0);
        if (0 != cam->latency_TotalShutterLagStart)
        {
        	cam->mlatency_Hsmcamera.e_OMXCAM_TotalShutterLag.t0 = cam->p_cam_sm->mTime;
        	cam->latency_TotalShutterLagStart = 0;
        	OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_TotalShutterLag %d", 0);
        }
        if(cam->mlatency_Hsmcamera.count_SerialShooting < IFM_MAX_SIZE_SERIALSHOOTING)
        {
           	cam->p_cam_sm->latencyMeasure(&(cam->p_cam_sm->mTime));
           	cam->mlatency_Hsmcamera.e_OMXCAM_SerialShooting[cam->mlatency_Hsmcamera.count_SerialShooting].t0 = cam->p_cam_sm->mTime;
           	cam->mlatency_Hsmcamera.e_OMXCAM_SerialShooting[cam->mlatency_Hsmcamera.count_SerialShooting].t1 = cam->p_cam_sm->mTime;
           	OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_SerialShooting %d", 0);
           	OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_SerialShooting %d", 1);
        	cam->mlatency_Hsmcamera.count_SerialShooting +=1;
        }
        //performance traces end

    }
//	ISP_VALID_BMS_FRAME_NOTIFICATION send out notification to stopVideo for Still during video.
	if(cam->p_cam_sm->GrabStatusNonDefferred==1)
	{		
		s_scf_event event;
		event.sig = HIDDEN_BMS_LAUNCHED_SIG;
		event.type.other = 1;		
		//Done to handle grababorted in requested state done for ZSL raw and Still during Video.
		cam->p_cam_sm->ProcessEvent(&event);	
	}

    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit Camera_ispctlInfo", cam);
    return;
}

OMX_ERRORTYPE Camera::suspendResources()
{
  MSG0("Camera::suspendResources\n");
  OstTraceFiltInst0(TRACE_DEBUG, "Camera::suspendResources");
    OMX_ERRORTYPE error=OMX_ErrorNone;
    /* free grab cache and stab buffers */
    error = iGrabControl.freeGrabCache();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("in suspendResources, couldn't free Grab Cache: omx error 0x%X\n", error);
        OstTraceFiltInst1(TRACE_ERROR, "in suspendResources, couldn't free Grab Cache: omx error 0x%X", error);
        return error;
    }
    error = iGrabControl.freeStabBuffers();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("in suspendResources, couldn't allocate stab buffers: omx error 0x%X\n", error);
        OstTraceFiltInst1(TRACE_ERROR, "in suspendResources, couldn't allocate stab buffers: omx error 0x%X", error);
        return error;
    }
    iGrabControl.mEsramSuspended=true;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE Camera::unsuspendResources()
{
    IN0("\n");
  MSG0("Camera::UnsuspendResources\n");
  OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::unsuspendResources");
  OstTraceFiltInst0(TRACE_DEBUG, "Camera::UnsuspendResources");

  iGrabControl.mEsramSuspended = false;
  if(iGrabControl.mWaitingForEsram)
  {
    /* Enqueue a deferred event to inform the port state machine that resources have been
       unsuspended by resource manager and hence we can reallocate grab cache & stab bfrs */
    s_scf_event event;
    event.type.other=0;
        event.sig = CAMERA_UNSUSPEND_RESOURCES_SIG;
        iDeferredEventMgr.queueNewEvent(&event);

        s_scf_event devent;
        devent.sig = Q_DEFERRED_EVENT_SIG;
        p_cam_sm->ProcessEvent(&devent);
  }

       OUTR(" ",(OMX_ErrorNone));
    OstTraceFiltInst1(TRACE_FLOW, "Exit unsuspendResources (%d)", OMX_ErrorNone);
    return OMX_ErrorNone;
}

/*
  called from COmxCamera::NmfPanicCallback in case of NMF panic on SIA.
  Brutally destroy the whole NMF network on DSP side
  Note that in this function we FORCE all components to stop and destroy
    whatever the error code returned by NMF functions !
*/
void Camera::errorRecovery()
{
    MSG0("OMX.HSMCAMERA Entering error recovery ---- destroying NMF network\n");
    OstTraceFiltInst0(TRACE_DEBUG, "OMX.HSMCAMERA Entering error recovery ---- destroying NMF network");
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();

    iPanic = OMX_TRUE;
    error = suspendResources(); /* this will free ESRAM */
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("Camera::errorRecovery : suspendResources error 0x%x\n", error);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : suspendResources error 0x%x", error);
    }
   /* mpImgnetwork can be NULL in certain cases of nmfpanic, we need to do a NULL check
   after forcing other NMF components to be closed */
   // mpImgNetwork->stop();

    /* stop & deinstantiate ISPCTL */
    error = iIspctlComponentManager.forcedStopNMF();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("Camera::errorRecovery : iIspctlComponentManager.forcedStopNMF error 0x%x\n", error);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : iIspctlComponentManager.forcedStopNMF error 0x%x", error);
    }
    error = iIspctlComponentManager.deinstantiateNMF();
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("Camera::errorRecovery : iIspctlComponentManager.deinstantiateNMF error 0x%x\n", error);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : iIspctlComponentManager.deinstantiateNMF error 0x%x", error);
    }

    /* For the case that something else is causing NMF panic, such as admserver */
    if (mpImgNetwork == NULL) {
        MSG("NMF panic happens! OMX.ST.HSMCamera error recovery ----mpImgNetwork  == NULL, aborting error recovery! \n");
        iIspctlCom.setPanic(OMX_TRUE);
        return;
    }
    mpImgNetwork->stop();

    /* deinstantiate NMF Network*/
    t_nmf_error err;
    err = mpImgNetwork->unbindFromUser("sendcommand");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : iIspctlComponentManager.forcedStopNMF error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : iIspctlComponentManager.forcedStopNMF error 0x%x", err);
    }
    err = mpImgNetwork->unbindFromUser("fsminit");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : unbindFromUser fsminit error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : unbindFromUser fsminit error 0x%x", err);
    }
    err = mpImgNetwork->unbindFromUser("fillthisbuffer[0]");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : unbindFromUser fillthisbuffer[0] error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : unbindFromUser fillthisbuffer error 0x%x", err);
    }
    err = mpImgNetwork->unbindFromUser("fillthisbuffer[1]");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : unbindFromUser fillthisbuffer[1] error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : unbindFromUser fillthisbuffer[1] error 0x%x", err);
    }
    err = mpImgNetwork->unbindFromUser("fillthisbuffer[2]");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : unbindFromUser fillthisbuffer[2] error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : unbindFromUser fillthisbuffer[2] error 0x%x", err);
    }
    err = mpImgNetwork->unbindFromUser("grabctl_configure");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : unbindFromUser grabctl_configure error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : unbindFromUser grabctl_configure error 0x%x", err);
    }
    err = mpImgNetwork->unbindFromUser("grab_command");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : unbindFromUser grab_command error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : unbindFromUser grab_command error 0x%x", err);
    }
    err = mpImgNetwork->unbindFromUser("stab_configure");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : unbindFromUser stab_configure error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : unbindFromUser stab_configure error 0x%x", err);
    }

    err = EnsWrapper_unbindToUser(handle, mpImgNetwork, "proxy");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : EnsWrapper_unbindToUser proxy error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : EnsWrapper_unbindToUser proxy error 0x%x", err);
    }
    err = EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[0]");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : EnsWrapper_unbindToUser fillbufferdone[0] error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : EnsWrapper_unbindToUser fillbufferdone[0] error 0x%x", err);
    }
    err = EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[1]");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : EnsWrapper_unbindToUser fillbufferdone[1] error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : EnsWrapper_unbindToUser fillbufferdone[1] error 0x%x", err);
    }
    err = EnsWrapper_unbindToUser(handle, mpImgNetwork, "fillbufferdone[2]");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : EnsWrapper_unbindToUser fillbufferdone[2] error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : EnsWrapper_unbindToUser fillbufferdone[2] error 0x%x", err);
    }
    err = EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_acknowledges");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : EnsWrapper_unbindToUser grabctl_acknowledges error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : EnsWrapper_unbindToUser grabctl_acknowledges error 0x%x", err);
    }
    err = EnsWrapper_unbindToUser(handle, mpImgNetwork, "grabctl_error");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : EnsWrapper_unbindToUser grabctl_error error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : EnsWrapper_unbindToUser grabctl_error error 0x%x", err);
    }
    err = EnsWrapper_unbindToUser(handle, mpImgNetwork, "grab_alert");
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : EnsWrapper_unbindToUser grab_alert error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : EnsWrapper_unbindToUser grab_alert error 0x%x", err);
    }
    err = mpImgNetwork->destroy();
    if (NMF_OK != err) {
        DBGT_ERROR("Camera::errorRecovery : mpImgNetwork destroy error 0x%x\n", err);
        OstTraceFiltInst1(TRACE_ERROR, "Camera::errorRecovery : mpImgNetwork destroy error 0x%x", err);
    }
    img_networkDestroy(mpImgNetwork);

    iIspctlCom.setPanic(OMX_TRUE);

    MSG0("OMX.HSMCAMERA error recovery ---- NMF network destroyed\n");
    OstTraceFiltInst0(TRACE_DEBUG, "OMX.HSMCAMERA error recovery ---- NMF network destroyed");
}

OMX_ERRORTYPE Camera::configureDamper(OMX_INDEXTYPE nIndex)
{
	IN0("\n");
	MSG0("Camera::configureDamper\n");
	//OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::configureDamper);

	OMX_ERRORTYPE error;
	error = OMX_ErrorNone;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
        return error;
#endif
	COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
	CTuning* pTuning = iOpModeMgr.GetOpModeTuning();
	e_iqset_id iqSet = IQSET_UNKNOWN;
	OMX_BOOL bDamperNeeded = OMX_TRUE;

    switch((t_uint32)nIndex)
    {
    	case OMX_IndexConfigCommonWhiteBalance:
    	{
    		switch (OMXCam->mWhiteBalControl.eWhiteBalControl)
    		{
    			case OMX_WhiteBalControlAuto:
    				iqSet = IQSET_WHITEBALANCE_AUTO;
    			break;
				case OMX_WhiteBalControlOff:
					bDamperNeeded =OMX_FALSE;
				break;
				case OMX_WhiteBalControlSunLight:
					iqSet = IQSET_WHITEBALANCE_DAYLIGHT;
				break;
				case OMX_WhiteBalControlCloudy:
					iqSet = IQSET_WHITEBALANCE_CLOUDY;
				break;
				case OMX_WhiteBalControlShade:
					iqSet = IQSET_WHITEBALANCE_SHADE;
				break;
				case OMX_WhiteBalControlTungsten:
					iqSet = IQSET_WHITEBALANCE_TUNGSTEN;
				break;
				case OMX_WhiteBalControlFluorescent:
					iqSet = IQSET_WHITEBALANCE_FLUORESCENT;
				break;
				case OMX_WhiteBalControlIncandescent:
					iqSet = IQSET_WHITEBALANCE_INCANDESCENT;
				break;
				case OMX_WhiteBalControlFlash:
					iqSet = IQSET_WHITEBALANCE_FLASH;
				break;
				case OMX_WhiteBalControlHorizon:
					iqSet = IQSET_WHITEBALANCE_HORIZON;
				break;
				default :
					bDamperNeeded =OMX_FALSE;
					MSG0("Camera::configureDamper OMX_ErrorBadParameter \n");
				return OMX_ErrorBadParameter;
    		}
    	} /*OMX_IndexConfigCommonWhiteBalance*/
    	break;
		case OMX_Symbian_IndexConfigSceneMode:
		{
			switch ((t_uint32)OMXCam->mSceneMode.eSceneType)
			{

				case OMX_SYMBIAN_SceneAuto:
            		iqSet = IQSET_SYMBIAN_SCENEAUTO;
            	break;

            	case OMX_SYMBIAN_ScenePortrait:
            		iqSet = IQSET_SYMBIAN_SCENEPORTRAIT;
            	break;

            	case OMX_SYMBIAN_SceneLandscape:
            		iqSet = IQSET_SYMBIAN_SCENELANDSCAPE;
            	break;

            	case OMX_SYMBIAN_SceneNight:
            		iqSet = IQSET_SYMBIAN_SCENENIGHT;
            	break;

            	case OMX_SYMBIAN_SceneNightPortrait:
            		iqSet = IQSET_SYMBIAN_SCENENIGHTPORTRAIT;
            	break;

            	case OMX_SYMBIAN_SceneSport:
            		iqSet = IQSET_SYMBIAN_SCENESPORT;
            	break;

            	case OMX_SYMBIAN_SceneMacro:
            		iqSet = IQSET_SYMBIAN_SCENEMACRO;
            	break;

            	case OMX_STE_Scene_Document:
            		iqSet = IQSET_STE_SCENEDOCUMENT;
            	break;

            	case OMX_STE_Scene_Beach:
            		iqSet = IQSET_STE_SCENEBEACH;
            	break;

            	case OMX_STE_Scene_Snow:
            		iqSet = IQSET_STE_SCENESNOW;
            	break;

            	case OMX_STE_Scene_Party:
            		iqSet = IQSET_STE_SCENEPARTY;
            	break;

            	case OMX_STE_Scene_Aqua:
            		iqSet = IQSET_STE_SCENEAQUA;
            	break;

            	case OMX_STE_Scene_Candlelight:
            		iqSet = IQSET_STE_SCENECANDLELIGHT; 
            	break;

            	case OMX_STE_Scene_Backlight:
            		iqSet = IQSET_STE_SCENEBACKLIGHT;
            	break;

            	case OMX_STE_Scene_Sunset:
            		iqSet = IQSET_STE_SCENESUNSET;
            	break;

            	default:
            		bDamperNeeded =OMX_FALSE;
					MSG0("Camera::configureDamper OMX_ErrorBadParameter \n");
				return OMX_ErrorBadParameter;

			}

		} /*OMX_Symbian_IndexConfigSceneMode*/
		break;
		case OMX_Symbian_IndexConfigFlickerRemoval:
		{
			switch (OMXCam->mFlickerRemoval.eFlickerRemoval)
			{
				case OMX_SYMBIAN_FlickerRemovalOff:
            		iqSet =  IQSET_FLICKERREJECTION_OFF;
            	break;
            	case OMX_SYMBIAN_FlickerRemoval50:
            		iqSet = IQSET_FLICKERREJECTION_50;
            	break;
				case OMX_SYMBIAN_FlickerRemoval60:
            		iqSet =  IQSET_FLICKERREJECTION_60;
            	break;
				case OMX_SYMBIAN_FlickerRemovalAuto:
            	default:
					bDamperNeeded =OMX_FALSE;
					MSG0("Camera::configureDamper OMX_ErrorBadParameter \n");
					return OMX_ErrorBadParameter;
			}
				
		}/*OMX_Symbian_IndexConfigFlickerRemoval*/
		break;
		case OMX_IndexConfigCommonExposureValue:
		{
			switch((t_uint32)OMXCam->mExposureValue.eMetering)
			{
				case OMX_MeteringModeAverage:
					iqSet = IQSET_AECMETERING_AVERAGE;
				break;
				case OMX_MeteringModeSpot:
					iqSet = IQSET_AECMETERING_SPOT;
				break;
				case OMX_MeteringModeMatrix:
					iqSet = IQSET_AECMETERING_MATRIX;
				break;
				case OMX_STE_MeteringModeCenterWeighted:
					iqSet = IQSET_AECMETERING_CENTERWEIGHTED;
				break;
				case OMX_STE_MeteringModeBeachAndSnow:
					iqSet = IQSET_AECMETERING_BEACH_SNOW;
				break;
				case OMX_STE_MeteringModeScenery:
					iqSet = IQSET_AECMETERING_AVERAGE;
				break;
				case OMX_STE_MeteringModePortrait:
					iqSet = IQSET_AECMETERING_PORTRAIT;
				break;
				default:
					bDamperNeeded =OMX_FALSE;
				break;
			}
			/*Since for 1 index 2 IQSets calling configure damper here*/
			if (OMX_TRUE == bDamperNeeded)
			{
				//printf("iIspDampers.Configure called  for index= 0x%x iqset=%d\n",nIndex,iqSet);
				t_isp_dampers_error_code eErr = iIspDampers.Configure( pTuning, iqSet);
				if(eErr!=ISP_DAMPERS_OK) {
					MSG2("Failed to configure ISP dampers: err=%d (%s)\n", eErr, CIspDampers::ErrorCode2String(eErr));
					DBC_ASSERT(0);
					error= OMX_ErrorNotImplemented;		
				}
			}
			if(OMXCam->mExposureValue.bAutoSensitivity ==OMX_TRUE)
			{
				iqSet = IQSET_ISO_AUTO;
			}
			else
			{
				if(OMXCam->mExposureValue.nSensitivity <=150){
					iqSet = IQSET_ISO_100;
				} else if(OMXCam->mExposureValue.nSensitivity <=300){
					iqSet = IQSET_ISO_200;
				} else if(OMXCam->mExposureValue.nSensitivity <=600){
					iqSet = IQSET_ISO_400;
				} else if(OMXCam->mExposureValue.nSensitivity <=1200){
					iqSet = IQSET_ISO_800;
				}else if(OMXCam->mExposureValue.nSensitivity <=2400){
					iqSet = IQSET_ISO_1600;
				}else {
					iqSet = IQSET_ISO_3200;
				}
			}
		}/*end of OMX_IndexConfigCommonExposureValue*/
		break;
		case OMX_IndexConfigCommonExposure:
		{
			switch ((t_uint32)OMXCam->mExposureControl.eExposureControl)
			{
				case OMX_ExposureControlOff:
					bDamperNeeded =OMX_FALSE;
				break;
				case OMX_ExposureControlAuto:
					iqSet = IQSET_EXPOSURECONTROL_AUTO;
				break;
				case OMX_ExposureControlNight:
					iqSet = IQSET_EXPOSURECONTROL_NIGHT;
				break;
				case OMX_ExposureControlBackLight:
					iqSet = IQSET_EXPOSURECONTROL_BACKLIGHT;
				break;
				case OMX_ExposureControlSpotLight:
					iqSet = IQSET_EXPOSURECONTROL_SPOTLIGHT;
				break;
				case OMX_ExposureControlSports:
					iqSet = IQSET_EXPOSURECONTROL_SPORTS;
				break;
				case OMX_ExposureControlSnow: 
					iqSet =  IQSET_EXPOSURECONTROL_SNOW;
				break;

				case OMX_ExposureControlBeach:
					iqSet = IQSET_EXPOSURECONTROL_BEACH;
				break;

				case OMX_ExposureControlLargeAperture:
					iqSet = IQSET_EXPOSURECONTROL_LARGEAPERTURE;
				break;

				case OMX_ExposureControlSmallApperture:
					iqSet = IQSET_EXPOSURECONTROL_SMALLAPERTURE;
				break;

				case OMX_SYMBIAN_ExposureControlCenter:
					iqSet = IQSET_EXPOSURECONTROL_CENTER;
				break;

				case OMX_SYMBIAN_ExposureControlVeryLong:
					iqSet = IQSET_EXPOSURECONTROL_VERYLONG;
				break;

				case OMX_SYMBIAN_ExposureControlHwFunctionalTesting:
					bDamperNeeded =OMX_FALSE;
				break;          
				default:
					bDamperNeeded =OMX_FALSE;
					MSG0("Camera::configureDamper OMX_ErrorBadParameter \n");
				return OMX_ErrorBadParameter;
			}
		}/*OMX_IndexConfigCommonExposure*/
		break;
		case OMX_IndexConfigCommonImageFilter:
		{
			switch ((t_uint32)OMXCam->mImageFilterType.eImageFilter)
			{
				case OMX_ImageFilterNone:
					iqSet = IQSET_IMAGEFILTER_NONE;
				break;
				case OMX_ImageFilterNegative:
					iqSet = IQSET_IMAGEFILTER_NEGATIVE;
				break;
				case OMX_SYMBIAN_ImageFilterSepia:
					iqSet = IQSET_IMAGEFILTER_SEPIA;
				break;
				case OMX_SYMBIAN_ImageFilterGrayScale:
					iqSet = IQSET_IMAGEFILTER_GRAYSCALE;
				break;
				case OMX_SYMBIAN_ImageFilterNatural:
					iqSet = IQSET_IMAGEFILTER_NATURAL;
				break;
				case OMX_SYMBIAN_ImageFilterVivid:
					iqSet = IQSET_IMAGEFILTER_VIVID;
				break;
				case OMX_ImageFilterSolarize:
					iqSet = IQSET_IMAGEFILTER_SOLARIZE;
				break;
				case OMX_ImageFilterEmboss:
					iqSet = IQSET_IMAGEFILTER_EMBOSS;
				break;
				case OMX_STE_ImageFilterWatercolor:
					iqSet = IQSET_IMAGEFILTER_WATERCOLOR;
				break;
				case OMX_STE_ImageFilterPastel:
					iqSet = IQSET_IMAGEFILTER_PASTEL;
				break;
				case OMX_STE_ImageFilterFilm:
					iqSet = IQSET_IMAGEFILTER_FILM;
				break;
				case OMX_STE_ImageFilterGrayscaleNegative:
					iqSet = IQSET_IMAGEFILTER_GRAYSCALENEGATIVE;
				break;
				case OMX_STE_ImageFilterPosterize:
					iqSet = IQSET_IMAGEFILTER_POSTERIZE;
				break;
				case OMX_ImageFilterBeach:
					iqSet =  IQSET_IMAGEFILTER_BEACH;
				break;
				case OMX_ImageFilterSnow:
					iqSet =  IQSET_IMAGEFILTER_SNOW;
				break;
				default:
					bDamperNeeded =OMX_FALSE;
					MSG0("Camera::configureDamper OMX_ErrorBadParameter \n");
				return OMX_ErrorBadParameter;
			}
		} /*OMX_IndexConfigCommonImageFilter */
		break;
		case OMX_Symbian_IndexConfigFocusRange:
		{
			switch (OMXCam->mFocusRange.eFocusRange)
			{
				case OMX_SYMBIAN_FocusRangeAuto:
					iqSet = IQSET_FOCUSRANGE_AUTO;
				break;
				case OMX_SYMBIAN_FocusRangeHyperfocal:
					iqSet = IQSET_FOCUSRANGE_HYPERFOCAL; 
				break;
				case OMX_SYMBIAN_FocusRangeNormal:
					iqSet = IQSET_FOCUSRANGE_NORMAL;
					break;
				case OMX_SYMBIAN_FocusRangeSuperMacro:
					iqSet = IQSET_FOCUSRANGE_SUPERMACRO;
				break;
				case OMX_SYMBIAN_FocusRangeMacro:
					iqSet = IQSET_FOCUSRANGE_MACRO;
				break;
				case OMX_SYMBIAN_FocusRangeInfinity:
					iqSet = IQSET_FOCUSRANGE_INFINITY; 
				break;
				default:
					bDamperNeeded =OMX_FALSE;
					MSG0("Camera::configureDamper OMX_ErrorBadParameter \n");
					return OMX_ErrorBadParameter;            	
			}
		} /*OMX_Symbian_IndexConfigFocusRange*/
		break;
		case OMX_Symbian_IndexConfigFocusRegion:
		{
			switch ((t_uint32)OMXCam->mFocusRegion.eFocusRegionControl)
			{
				case OMX_SYMBIAN_FocusRegionAuto:
					iqSet = IQSET_FOCUSREGION_AUTO; 
					break;
				case OMX_STE_FocusRegionCenter:
					iqSet = IQSET_FOCUSREGION_CENTER; 
				break;
				case OMX_SYMBIAN_FocusRegionManual:
					iqSet = IQSET_FOCUSREGION_MANUAL; 
				break;
				case OMX_SYMBIAN_FocusRegionFacePriority:
            	case OMX_SYMBIAN_FocusRegionObjectPriority:
            	case OMX_STE_FocusRegionTouchPriority:
					bDamperNeeded =OMX_FALSE;
					break;
				default:
					bDamperNeeded =OMX_FALSE;
					MSG0("Camera::configureDamper OMX_ErrorBadParameter \n");
					return OMX_ErrorBadParameter;   
			
			}
			
		} /*OMX_Symbian_IndexConfigFocusRegion*/
		break;
		default:
			/*Indexes for dampers are not needed*/
			/*Ideally there should not be any call*/
			bDamperNeeded =OMX_FALSE;
			break;
	} 

	if (OMX_TRUE == bDamperNeeded)
	{
		//printf("iIspDampers.Configure called  for index= 0x%x iqset=%d\n",nIndex,iqSet);
		t_isp_dampers_error_code eErr = iIspDampers.Configure( pTuning, iqSet);
		if(eErr!=ISP_DAMPERS_OK) {
			MSG2("Failed to configure ISP dampers: err=%d (%s)\n", eErr, CIspDampers::ErrorCode2String(eErr));
			DBC_ASSERT(0);
			error= OMX_ErrorNotImplemented;		
		}
	}
	
	OUTR(" ",(error));
	OstTraceFiltInst1(TRACE_FLOW, "Exit ConfigureDamper (%d)", error);
	return error;
}

/*called from Camera::applyConfig
   nIndex specifies the OMX index for which Damper configuration needs to be done. This 
   function just registers the index id, and later on in teh preview/rawcapture state machine
   the actual Damper configuration is done for the registered indexes*/
OMX_ERRORTYPE Camera::addIndexToDamperConfigList(OMX_INDEXTYPE nIndex)
{
	switch ((t_uint32)nIndex)
	{
		case OMX_IndexConfigCommonWhiteBalance:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_WHITE_BAL].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_WHITE_BAL].nIndex = nIndex;
			break;
              case OMX_IndexConfigCommonExposureValue:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_EXPOSURE_VAL].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_EXPOSURE_VAL].nIndex = nIndex;
			break;
              case OMX_IndexConfigCommonExposure:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_EXPOSURE].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_EXPOSURE].nIndex = nIndex;
			break;
              case OMX_IndexConfigCommonImageFilter:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_IMAGE_FILTER].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_IMAGE_FILTER].nIndex = nIndex;
			break;
              case OMX_Symbian_IndexConfigFocusRegion:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_FOCUS_REGION].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_FOCUS_REGION].nIndex = nIndex;
			break;
              case OMX_Symbian_IndexConfigFocusRange:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_FOCUS_RANGE].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_FOCUS_RANGE].nIndex = nIndex;
			break;
              case OMX_Symbian_IndexConfigSceneMode:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_SCENE_MODE].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_SCENE_MODE].nIndex = nIndex;
			break;
              case OMX_Symbian_IndexConfigFlickerRemoval:
			isDamperConfigReqd[OMX_DAMPER_CONFIG_FLICKER_REMOVAL].bIndexToBeConfigured= OMX_TRUE;
			isDamperConfigReqd[OMX_DAMPER_CONFIG_FLICKER_REMOVAL].nIndex = nIndex;
			break;
		default:
			return OMX_ErrorBadParameter;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE Camera::printPrivateContext(OMX_OTHER_PARAM_PRIVATE_CONTEXT *apCtx)
{
    IN0();
    OstTraceFiltInst0(TRACE_FLOW, "Entry Camera::printPrivateContext");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (apCtx == NULL) {
        OUTR("Null Argument",(OMX_ErrorBadParameter));
        OstTraceFiltInst0(TRACE_FLOW, "Exit Camera::printPrivateContext Null Argument");
        return OMX_ErrorBadParameter;
    }

    t_ContextDebugPrint CtxLog = apCtx->pPrint;
    OMX_PTR pPvt = apCtx->pPrintPrivate;

    CtxLog(pPvt, "|-[CameraStatus]\n");
    CtxLog(pPvt, "| |- operating_mode %s\n", iOpModeMgr.getOpModeStr());
    if (p_cam_sm != NULL)
        CtxLog(pPvt, "| |- stateMachine %p\n", p_cam_sm->getCurrentState());

    CtxLog(pPvt, "| |- frameRateType %s\n", (iFramerate.isFixedFramerate() == true) ? "Fixed" : "Variable");

    if (iFramerate.isFixedFramerate()) {
        CtxLog(pPvt, "| |-framerate %d\n", iFramerate.getSensorFramerate_x100());
    }

    //print grab ports status
    CtxLog(pPvt, "|-[GrabStatus]\n");

    for (int i = 0; i < CAMERA_NB_OUT_PORTS; i++) {
        CtxLog(pPvt, "| |-[Pipe %d \n", i);
        CtxLog(pPvt, "| | |- PortMapped %s\n", stringifyGrabPipeConfig(grabPipeIDConfig[i]));
        CtxLog(pPvt, "| | |- pipeStatus %s\n", iGrabControl.stringfyGrabStatus(i));
        CtxLog(pPvt, "| | |- nBufferAtMPC %d\n", iOpModeMgr.nBufferAtMPC[i]);
        if (grabErrorDetail[i].error_id != 0)
            CtxLog(pPvt, "| |- error_id 0x%X data 0x%x\n",
                   grabErrorDetail[i].error_id,
                   grabErrorDetail[i].data);
    }

    if (p_cam_sm != NULL) {
        CtxLog(pPvt, "|-[ISP Status]\n");
        CtxLog(pPvt, "| |- Pipe0_XSize %u\n", p_cam_sm->mIspPrivateData.Pipe0_XSize);
        CtxLog(pPvt, "| |- Pipe0_YSize %u\n", p_cam_sm->mIspPrivateData.Pipe0_YSize);
        CtxLog(pPvt, "| |- Pipe1_XSize %u\n", p_cam_sm->mIspPrivateData.Pipe1_XSize);
        CtxLog(pPvt, "| |- Pipe1_YSize %u\n", p_cam_sm->mIspPrivateData.Pipe1_YSize);
        CtxLog(pPvt, "| |- Pipe0_OutputFormat %s\n",
               stringifyISPOutputFormat((OutputFormat_te)p_cam_sm->mIspPrivateData.Pipe0_OutputFormat));
        CtxLog(pPvt, "| |- Pipe1_OutputFormat %s\n",
               stringifyISPOutputFormat((OutputFormat_te)p_cam_sm->mIspPrivateData.Pipe1_OutputFormat));
        CtxLog(pPvt, "| |- Pipe0_Status %s\n",
               p_cam_sm->mIspPrivateData.Pipe0_Enable == true ? "Enabled" : "Disabled");
        CtxLog(pPvt, "| |- Pipe1_Status %s\n",
               p_cam_sm->mIspPrivateData.Pipe1_Enable == true ? "Enabled" : "Disabled");
        CtxLog(pPvt, "| |- ISPLowLevelStatus %s\n",
               stringifyISPState((HostInterfaceLowLevelState_te)p_cam_sm->mIspPrivateData.ISPLowLevelStatus));
    }

    CtxLog(pPvt, "|-[sw3A Status]\n");
    CtxLog(pPvt, "| |- state %s\n", i3AComponentManager.getCurrentSw3AStateStr());
    CtxLog(pPvt, "| |- lens %s\n", i3AComponentManager.mSw3AStatStatus.bLensMoved == 0 ? "Not Moved" : "Moved");
    CtxLog(pPvt, "| |- af stat requested %u\n",
           i3AComponentManager.mSw3AStatStatus.afMoveAndExportStatRequested
           + i3AComponentManager.mSw3AStatStatus.afExportStatRequested);
    CtxLog(pPvt, "| |- af stat received %u\n", i3AComponentManager.mSw3AStatStatus.afStatReceived);
    CtxLog(pPvt, "| |- glace stat requested %u\n", i3AComponentManager.mSw3AStatStatus.glaceStatRequested);
    CtxLog(pPvt, "| |- glace stat received %u\n", i3AComponentManager.mSw3AStatStatus.glaceStatReceived);

    OUTR(" ",(err));
    OstTraceFiltInst1(TRACE_FLOW, "Exit Camera::printPrivateContext (%d)", (err));
    return err;
}

#define STRINGIFY(mode) case mode: return #mode
const char* stringifyGrabPipeConfig(e_grabPipeID aGrabPipeID)
{
    switch (aGrabPipeID) {
        STRINGIFY(GRBPID_PIPE_LR);
        STRINGIFY(GRBPID_PIPE_HR);
        STRINGIFY(GRBPID_PIPE_RAW_OUT);
        STRINGIFY(GRBPID_PIPE_RAW_IN);
        STRINGIFY(GRBPID_PIPE_CAM);
        default: return "Invalid GrabPipeID";
    }
}

const char* stringifyISPState(HostInterfaceLowLevelState_te aHostInterfaceLowLevelState)
{
    switch (aHostInterfaceLowLevelState) {
        STRINGIFY(HostInterfaceLowLevelState_e_RAW_UNINITIALISED);
        STRINGIFY(HostInterfaceLowLevelState_e_BOOTING);
        STRINGIFY(HostInterfaceLowLevelState_e_STOPPED);
        STRINGIFY(HostInterfaceLowLevelState_e_DATA_PATH_SETUP);
        STRINGIFY(HostInterfaceLowLevelState_e_ALGORITHM_UPDATE);
        STRINGIFY(HostInterfaceLowLevelState_e_SENSOR_COMMIT);
        STRINGIFY(HostInterfaceLowLevelState_e_ISP_SETUP);
        STRINGIFY(HostInterfaceLowLevelState_e_COMMANDING_RUN);
        STRINGIFY(HostInterfaceLowLevelState_e_WAITING_FOR_RUN);
        STRINGIFY(HostInterfaceLowLevelState_e_RUNNING);
        STRINGIFY(HostInterfaceLowLevelState_e_COMMANDING_STOP);
        STRINGIFY(HostInterfaceLowLevelState_e_WAITING_FOR_STOP);
        STRINGIFY(HostInterfaceLowLevelState_e_GOING_TO_SLEEP);
        STRINGIFY(HostInterfaceLowLevelState_e_RESET_ISP);
        STRINGIFY(HostInterfaceLowLevelState_e_SLEPT);
        STRINGIFY(HostInterfaceLowLevelState_e_WAKING_UP);
        STRINGIFY(HostInterfaceLowLevelState_e_ENTERING_ERROR);
        STRINGIFY(HostInterfaceLowLevelState_e_ERROR);
        default: return "Invalid ISP state";
    }
}

const char* stringifyISPOutputFormat(OutputFormat_te outputFormat)
{
    switch (outputFormat) {
        STRINGIFY(OutputFormat_e_RGB101010_PEAKED);
        STRINGIFY(OutputFormat_e_RGB101010_UNPEAKED);
        STRINGIFY(OutputFormat_e_RGB888_PEAKED);
        STRINGIFY(OutputFormat_e_RGB888_UNPEAKED);
        STRINGIFY(OutputFormat_e_RGB565_PEAKED);
        STRINGIFY(OutputFormat_e_RGB565_UNPEAKED);
        STRINGIFY(OutputFormat_e_RGB555_PEAKED);
        STRINGIFY(OutputFormat_e_RGB555_UNPEAKED);
        STRINGIFY(OutputFormat_e_RGB444_UNPACKED_PEAKED);
        STRINGIFY(OutputFormat_e_RGB444_UNPACKED_UNPEAKED);
        STRINGIFY(OutputFormat_e_RGB444_PACKED_PEAKED);
        STRINGIFY(OutputFormat_e_RGB444_PACKED_UNPEAKED);
        STRINGIFY(OutputFormat_e_YUV);
        default: return "Invalid color format";
    }
}