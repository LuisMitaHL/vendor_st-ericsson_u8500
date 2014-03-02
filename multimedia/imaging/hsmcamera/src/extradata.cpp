/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "hsmcam.h"
#include "osi_trace.h"
#include "ImgConfig.h"
#include "omxilosalservices.h"
#include "extradata.h"
#include "camera.h"
#include "ExifReader.h"
#include <time.h>

void CAM_SM::extradataCircularBuffer_init()
{
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    if (NULL != pOpModeMgr) {
        DBC_ASSERT(pOpModeMgr->GetFrameBefore()<MAX_ALLOCATED_BUFFER_NB);

        EmptyExtradataBuffer = (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled()) ? OMX_FALSE : OMX_TRUE;
        ExtradataBufferSize = pOpModeMgr->GetFrameBefore();
        for (int i=0 ; i<MAX_ALLOCATED_BUFFER_NB ; i++) {
            error = Extradata::ClearAllExtradata(&ExtradataBuffer[i][0]);
            if (OMX_ErrorNone != error) {
                MSG0("CAM_SM::extradataCircularBuffer_init : failed to ClearAllExtradata\n");
            }
        }
    }
    else {
        EmptyExtradataBuffer = OMX_FALSE;
        ExtradataBufferSize = 0;
    }
    ExtradataBufferPtr = 0;
    ExtradataBufferElements = 0;
}

void CAM_SM::extradataCircularBuffer_startEmptying()
{
    if (OMX_FALSE == EmptyExtradataBuffer)
    {
        EmptyExtradataBuffer = OMX_TRUE;
        /* circular buffer is not full => first element is at the beginning of buffer */
        if (ExtradataBufferElements < ExtradataBufferSize) {
            ExtradataBufferPtr = 0;
        }
        /* circular buffer is full => pointer is already at first element, nothing to do */
        else {}
    }
}

OMX_BOOL CAM_SM::extradataCircularBuffer_isEmptying()
{
    return (EmptyExtradataBuffer);
}

OMX_U8* CAM_SM::extradataCircularBuffer_pop()
{
    OMX_U8* ptr = NULL;

    if (0 == ExtradataBufferSize) {
        return (ptr);
    }

    /* we want to add new extradatas to circular buffer */
    if (OMX_FALSE == EmptyExtradataBuffer) {
        ptr = &ExtradataBuffer[ExtradataBufferPtr][0];
        ExtradataBufferPtr++;
        ExtradataBufferPtr = ExtradataBufferPtr % ExtradataBufferSize;
        if (ExtradataBufferElements < ExtradataBufferSize) {
            ExtradataBufferElements++;
        }
    }
    /* we want to get extradatas to attach them to a buffer */
    else {
        /* circular buffer is empty */
        if (0 == ExtradataBufferElements) {
            ptr = NULL;
        }
        else {
            ptr = &ExtradataBuffer[ExtradataBufferPtr][0];
            ExtradataBufferPtr++;
            ExtradataBufferPtr = ExtradataBufferPtr % ExtradataBufferSize;
            if (0 != ExtradataBufferElements) {
                ExtradataBufferElements--;
            }
        }
    }

    return (ptr);
}


/* - inOMXBuffer is false : we write extradatas in memory (time nudge before capturing bit)
 * - inOMXBuffer is true :
 *        - there is extradatas in memory : we copy extradatas from memory to OMX buffer
 *             (time nudge after capturing bit for buffers grabbed before capturing bit)
 *        - there is no extradatas in memory : we write extradatas in OMX buffer
 *             (time nudge after capturing bit for buffers grabbed after capturing bit,
 *              "standard" still/burst capture)
 */
OMX_ERRORTYPE CAM_SM::setVPB1Extradata(OMX_BUFFERHEADERTYPE* pOmxBufHdr, bool inOMXBuffer)
{
    OMX_ERRORTYPE error = OMX_ErrorUndefined;
    OMX_U8* pMem = NULL;

    pMem = extradataCircularBuffer_pop();

    /* time nudge after capturing bit for buffers grabbed before capturing bit */
    /* we only need to copy extradatas already stored in memory */
    if ((OMX_TRUE == inOMXBuffer) && (NULL != pMem)) {
        MSG0("setVPB1Extradata : copy extradatas stored in memory\n");
        OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParameters;
        OMX_VERSIONTYPE version;

        Extradata::GetExtradataFieldFromMem(pMem, &CaptureParameters, version);
        CaptureParameters.eSceneOrientation = ((COmxCamera*)&mENSComponent)->mSceneOrientation.eOrientation;
        Extradata::SetExtradataFieldInMem(pMem, &CaptureParameters, version, 1);

        error = Extradata::CopyExtradata_Mem2Buffer(pOmxBufHdr, pMem, EXTRADATA_STILLPACKSIZE);
        if(OMX_ErrorNone != error) {
            return error;
        }

        OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;

        /*finding the memory occupied by structures before exif extradata*/
        OMX_U32 size_extradata_before_exif = ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(IFM_BMS_CAPTURE_CONTEXT) + 3) & ~0x3) +
                                            ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(IFM_DAMPERS_DATA) + 3 ) & ~0x3) +
                                            ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE) + 3) & ~0x3) +
                                            ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE) + 3) & ~0x3) +
                                            ((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE) + 3) & ~0x3);


        /*adding the size occupied by structures before exif extradata to reach exif extradata*/
        pCur +=  size_extradata_before_exif;
        OMX_OTHER_EXTRADATATYPE *ext_data = (OMX_OTHER_EXTRADATATYPE*)pCur;
        if(ext_data->eType == (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataEXIFAppMarker) {
            pCur = (OMX_U8*)(&(ext_data->data) + 8);
            ExifReader Reader;
            uint32_t maxsize=0;
            ExifPtrDataValue Ptr;
            error = (OMX_ERRORTYPE)Reader.AttachBuffer((char*)pCur, pOmxBufHdr->nAllocLen-(pOmxBufHdr->nFilledLen+size_extradata_before_exif));
            if (error == OMX_ErrorNone) {
                ExifTag *pTag;
                pTag=Reader.FindTag(eExifTag_Orientation);
                if (pTag!= NULL) {
                    pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
                    *Ptr.usValue= ((COmxCamera*)&mENSComponent)->mSceneOrientation.eOrientation;
                }
            }
        }
        return error;
    }

    /* compute extradatas and store them in correct place (OMX buffer or memory) */
    if ((OMX_TRUE == inOMXBuffer) || (0 != ExtradataBufferSize)) {
        /* Prepare extradata */
        OMX_VERSIONTYPE version;
        version.nVersion = OMX_VERSION;
        COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;

        if (false == inOMXBuffer) {
            error = Extradata::ClearAllExtradata(pMem);
        }
        else {
            error = Extradata::ClearAllExtradata(pOmxBufHdr);
        }
        if (OMX_ErrorNone != error) {
            return error;
        }

        /* Add user settings extradata */
        if (false == inOMXBuffer) {
            error = Extradata::SetExtradataFieldInMem(pMem, &pOmxCamera->mCameraUserSettings, version, CAMERA_PORT_OUT1);
        }
        else {
            error = Extradata::SetExtradataField(pOmxBufHdr, &pOmxCamera->mCameraUserSettings, version);
        }
        if (OMX_ErrorNone != error) {
            MSG0("setVPB1Extradata : Failed to add OMX User Settings extradata\n");
            return error;
        }

        /* Prepare BMS capture context extradata */
        IFM_BMS_CAPTURE_CONTEXT BMSCaptureContext;
        volatile float FieldOfView = pZoom->maxFOVXAtCurrentCenter * (0x10000/(float)pZoom->pZoomFactorStatus->xZoomFactor.nValue);
        BMSCaptureContext.Zoom_Control_f_SetFOVX = CIspctlCom::floattou32(FieldOfView);
        BMSCaptureContext.Zoom_Control_s16_CenterOffsetX = pZoom->pCenterFOVStatus->sPoint.nX;
        BMSCaptureContext.Zoom_Control_s16_CenterOffsetY = pZoom->pCenterFOVStatus->sPoint.nY;
        BMSCaptureContext.FrameDimensionStatus_u16_MaximumUsableSensorFOVX = pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVX;
        BMSCaptureContext.FrameDimensionStatus_u16_MaximumUsableSensorFOVY = pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVY;
        BMSCaptureContext.SensorFrameConstraints_u16_MaxOPXOutputSize = pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPXOutputSize;
        BMSCaptureContext.SensorFrameConstraints_u16_MaxOPYOutputSize = pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPYOutputSize;
        BMSCaptureContext.SensorFrameConstraints_u16_VTXAddrMin = pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMin;
        BMSCaptureContext.SensorFrameConstraints_u16_VTYAddrMin = pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMin;
        BMSCaptureContext.SensorFrameConstraints_u16_VTXAddrMax = pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMax;
        BMSCaptureContext.SensorFrameConstraints_u16_VTYAddrMax = pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMax;
        BMSCaptureContext.CurrentFrameDimension_f_PreScaleFactor = pCapture_context->nBMS_capture_context.CurrentFrameDimension_f_PreScaleFactor;
        BMSCaptureContext.CurrentFrameDimension_u16_VTXAddrStart = pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrStart;
        BMSCaptureContext.CurrentFrameDimension_u16_VTYAddrStart = pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrStart;
        BMSCaptureContext.CurrentFrameDimension_u16_VTXAddrEnd = pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrEnd;
        BMSCaptureContext.CurrentFrameDimension_u16_VTYAddrEnd = pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrEnd;
        BMSCaptureContext.PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun= pCapture_context->nBMS_capture_context.PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun;
        BMSCaptureContext.PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun= pCapture_context->nBMS_capture_context.PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun;
        BMSCaptureContext.CurrentFrameDimension_u8_NumberOfStatusLines= pCapture_context->nBMS_capture_context.CurrentFrameDimension_u8_NumberOfStatusLines;
        BMSCaptureContext.SystemConfig_Status_e_PixelOrder=pCapture_context->nBMS_capture_context.SystemConfig_Status_e_PixelOrder;
        BMSCaptureContext.TimeNudgeEnabled = pOpModeMgr->IsTimeNudgeEnabled();

        /* Add BMS capture context extradata */
        if (false == inOMXBuffer) {
            error = Extradata::SetExtradataFieldInMem(pMem, &BMSCaptureContext, version, CAMERA_PORT_OUT1);
        }
        else {
            error = Extradata::SetExtradataField(pOmxBufHdr, &BMSCaptureContext, version);
        }
        if (OMX_ErrorNone != error) {
            return error;
        }

        OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *pCaptureParameters = p3AComponentManager->getCaptureParameters(false);
        if (NULL == pCaptureParameters) {
            return(OMX_ErrorUndefined);
        }
        pCaptureParameters->eSceneOrientation = ((COmxCamera*)&mENSComponent)->mSceneOrientation.eOrientation;
        MSG1("setVPB1Extradata pCaptureParameters->orientation %d\n",pCaptureParameters->eSceneOrientation);
        /*  Add 3a extradatas */
        if (false == inOMXBuffer) {
            error = Extradata::SetExtradataFieldInMem(pMem, pCaptureParameters, version, CAMERA_PORT_OUT1);
        }
        else {
            error = Extradata::SetExtradataField(pOmxBufHdr, pCaptureParameters, version);
        }
        if (OMX_ErrorNone != error) {
            return error;
        }

        // Prepare capture parameters extradata
        OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE frameDesc;
        frameDesc.nPixelFormat           = 2; // compressed and unpacked (u16_CsiRawFormat)
        frameDesc.nDataEndianess         = 0; // little endian
        frameDesc.eRawPreset             = ePreset;//; raw raw,or ideal raw
        frameDesc.nColorOrder            = BMSCaptureContext.SystemConfig_Status_e_PixelOrder;        /**< The used color order, as in SMIA spec Table 44, e.g. 0 - GRBG, */
        frameDesc.nBitDepth              =(OMX_U8)-1;/* For the moment put a dummy value */ /**< The bit depth: 8, 10, 12, ... bit pixel data */
        frameDesc.nDataPedestal          = -1; /**< The data pedestal, -1 if N.A.  ===> to be given from ISP FW*/
        frameDesc.nFrameCounter          = BMSCaptureContext.PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun;
        frameDesc.nImageDataReadoutOrder = (OMX_U8)-1;/* For the moment put a dummy value */ //==> mirroring settings
        frameDesc.nIslLinesTop           = BMSCaptureContext.CurrentFrameDimension_u8_NumberOfStatusLines;
        frameDesc.nIslLinesBottom        = 0; // ===> to be given from ISP FW
        frameDesc.nVisColsStart          = 0; /**< Visible pixels sensor coordinates  providing also extras information */
        frameDesc.nVisCols               = BMSCaptureContext.FrameDimensionStatus_u16_MaximumUsableSensorFOVX;
        frameDesc.nVisRowsStart          = 0;
        frameDesc.nVisRows               = BMSCaptureContext.FrameDimensionStatus_u16_MaximumUsableSensorFOVY;
        frameDesc.nSpatialSampling       = 0; //0 for Bayer sampling,
        frameDesc.nScalingMode           = 0; /**< It is 0 for None, 1 horizontal, 2 full, ...   : esensorprofile */

        /* Add capture parameters extradata */
        if (false == inOMXBuffer) {
            error = Extradata::SetExtradataFieldInMem(pMem, &frameDesc, version, CAMERA_PORT_OUT1);
        }
        else {
            error = Extradata::SetExtradataField(pOmxBufHdr, &frameDesc, version);
        }
        if (OMX_ErrorNone != error) {
            return error;
        }

        // Get SW3A dampers data
        IFM_DAMPERS_DATA* pDampersData = NULL;
        pDampersData = p3AComponentManager->getDampersData(false);
        if( pDampersData == NULL ) {
            MSG0("setVPB1Extradata : Failed to get Damper Data from SW3A\n");
            return OMX_ErrorUndefined;
        }

            // Add the Dampers related extradata
        if (false == inOMXBuffer) {
            error = Extradata::SetExtradataFieldInMem(pMem, pDampersData, version, CAMERA_PORT_OUT1);
        }
        else {
            error = Extradata::SetExtradataField(pOmxBufHdr, pDampersData, version);
        }
        if (OMX_ErrorNone != error) {
            MSG0("setVPB1Extradata : Failed to set populate the Dampers Bases extradata\n");
            return error;
        }

        // Get current Date/Time
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
        // Get SW3A MakerNotes
        t_sw3A_Buffer* pMakerNotes = p3AComponentManager->getPendingMakernotes();
        t_sw3A_Buffer makerNotes;
        if(pMakerNotes == NULL) {
            MSG0("setVPB1Extradata: No makernotes available: pMakerNotes==NULL\n");
            makerNotes.pData = NULL;
            makerNotes.size = 0;
        }
        else {
            makerNotes = *pMakerNotes;
        }

        // Pack the capture parameters for EXIF
        camport* pCamPort = (camport*) mENSComponent.getPort(CAMERA_PORT_OUT1);
        IFM_EXIF_SETTINGS sExifSettings;
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
        sExifSettings.nPictureNumPixelsX   = (OMX_U32)(pCamPort->getParamPortDefinition().format.video.nStride/pCamPort->mBytesPerPixel); /* for raw format, nStride/BPP contains proper num of pixels */
        sExifSettings.nPictureNumPixelsY   = (OMX_U32)pCamPort->getParamPortDefinition().format.video.nSliceHeight; /* should be equal to nFrameHeight, but let's be consistent with previous use of nStride */
        sExifSettings.eWhiteBalanceControl = pOmxCamera->mWhiteBalControl.eWhiteBalControl;
        Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
        sExifSettings.fFocalLength         = ((Cam->mLensParameters.nFocalLength)/65536.0);
        sExifSettings.bPictureEnhanced     = pOmxCamera->mImageFilterType.eImageFilter == OMX_ImageFilterNone ? OMX_FALSE:OMX_TRUE;
        sExifSettings.eSceneType           = pOmxCamera->mSceneMode.eSceneType;
        sExifSettings.eMeteringMode        = pOmxCamera->mExposureValue.eMetering;
        sExifSettings.bFlashFired          = pCaptureParameters->bFlashFired;
        sExifSettings.eFlashControl        = pOmxCamera->mFlashControlStruct.eFlashControl;
        //Note: This boolean value is taken care of in ifm/src/exif.cpp. i.e. the meaning of this bit will be reversed there, to compy with EXIF spec
        sExifSettings.bFlashPresent        = (p3AComponentManager->mFlashController.isFlashModeSupported() == true) ? OMX_TRUE: OMX_FALSE;
        sExifSettings.nMaxNumPixelsX       = (OMX_U16)BMSCaptureContext.SensorFrameConstraints_u16_MaxOPXOutputSize;
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
	 sExifSettings.nZoomFactor	= ((pZoom->pZoomFactorStatus->xZoomFactor.nValue * 100)/0x10000);	//in Q16 format

        // Add EXIF extradata to current frame
        if (false == inOMXBuffer) {
            error = Extradata::SetExtradataFieldInMem(pMem, &sExifSettings, version, CAMERA_PORT_OUT1);
        }
        else {
            error = Extradata::SetExtradataField(pOmxBufHdr, &sExifSettings, version);
        }
        if (OMX_ErrorNone != error) {
            MSG0("setVPB1Extradata: Failed to set the EXIF extradata\n");
            return error;
        }

        OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraCaptureParameters = p3AComponentManager->getSteExtraCaptureParameters(false);
        if (NULL == pSteExtraCaptureParameters) {
            return(OMX_ErrorUndefined);
        }

        // Add the Ste Extra Capture related extradata
        if (false == inOMXBuffer) {
            error = Extradata::SetExtradataFieldInMem(pMem, pSteExtraCaptureParameters, version, CAMERA_PORT_OUT1);
        }
        else {
            error = Extradata::SetExtradataField(pOmxBufHdr, pSteExtraCaptureParameters, version);
        }
        if (OMX_ErrorNone != error)
        {
            MSG0("Failed to set populate the steExtraCapture data\n");
            return error;
        }
        /*Resetting valid values to False */
        pSteExtraCaptureParameters->bValidGamma = OMX_FALSE;
        pSteExtraCaptureParameters->bValidEffect = OMX_FALSE;
        pSteExtraCaptureParameters->bValidSFX = OMX_FALSE;

        // MakerNotes are now consumed => we can release them
        p3AComponentManager->releasePendingMakernotes(pMakerNotes);
    }

    return (OMX_ErrorNone);
}


